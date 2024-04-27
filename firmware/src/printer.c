/**
 * Copyright (C) 2024 Oliver Larsson
 *
 * This file is part of si-printer-bridge.
 *
 * si-printer-bridge is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * si-printer-bridge is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * si-printer-bridge. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <tusb.h>

#include "common/tusb_types.h"
#include "console.h"
#include "printer.h"

// Printer commands
// https://reference.epson-biz.com/modules/ref_escpos/

#define PRINTER_INIT 0x1B40
#define PRINTER_FEED 0x1B64

#define PRINTER_MODE_SET 0x1B21
#define PRINTER_MODE_BOLD (1 << 3)
#define PRINTER_MODE_DHEIGHT (1 << 4)
#define PRINTER_MODE_DWIDTH (1 << 5)
#define PRINTER_MODE_UNDERLINE (1 << 7)

#define PRINTER_CHAR_SET 0x1B52
#define PRINTER_CHAR_SET_SWE 0x05

#define PRINTER_JUSTIFICATION 0x1B61
#define PRINTER_JUSTIFICATION_LEFT 0x00
#define PRINTER_JUSTIFICATION_CENTER 0x01
#define PRINTER_JUSTIFICATION_RIGHT 0x02

#define USB_FRAME_SIZE 64

// Only allow printing if initialized.
static int s_initialized = 0;
static tuh_xfer_t s_xfer = {0};
static uint8_t s_print_buffer[2048];
static int s_printed = 0;
static void (*s_done_cb)(void) = NULL;

void printer_xfer_callback(tuh_xfer_t *xfer);

static uint16_t count_interface_total_len(const tusb_desc_interface_t *desc_itf,
                                          uint8_t itf_count, uint16_t max_len);

static int open_printer_interface(uint8_t dev_addr,
                                  const tusb_desc_interface_t *desc_itf,
                                  uint16_t max_len);

static inline void write_command(int *offset, uint16_t command);
static void write(int *offset, const char *__restrict, ...)
    __attribute__((__format__(__printf__, 2, 3)));

// Initialize a printer. Returns 0 on success.
int printer_init(uint8_t dev_addr) {
  s_initialized = 0;

  uint8_t buffer[256];
  tusb_desc_configuration_t *desc_configuration =
      (tusb_desc_configuration_t *)buffer;

  uint8_t res = tuh_descriptor_get_configuration_sync(dev_addr, 0, &buffer,
                                                      sizeof(buffer));

  if (res != XFER_RESULT_SUCCESS)
    return -1;

  const uint8_t *desc_end = (const uint8_t *)desc_configuration +
                            tu_le16toh(desc_configuration->wTotalLength);

  const uint8_t *p_desc = tu_desc_next(desc_configuration);

  while (p_desc < desc_end) {
    uint8_t assoc_itf_count = 1;

    if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE_ASSOCIATION) {
      const tusb_desc_interface_assoc_t *desc_iad =
          (const tusb_desc_interface_assoc_t *)p_desc;
      assoc_itf_count = desc_iad->bInterfaceCount;
      p_desc = tu_desc_next(p_desc);
    }

    if (tu_desc_type(p_desc) != TUSB_DESC_INTERFACE)
      return -1;

    const tusb_desc_interface_t *desc_itf =
        (const tusb_desc_interface_t *)p_desc;

    const uint16_t drv_len = count_interface_total_len(
        desc_itf, assoc_itf_count, (desc_end - p_desc));

    if (drv_len < sizeof(tusb_desc_interface_t))
      return -1;

    console_printf("  Interface: %u\r\n", desc_itf->bInterfaceNumber);
    console_printf("    Class: 0x%02x\r\n", desc_itf->bInterfaceClass);
    console_printf("    Subclass: 0x%02x\r\n", desc_itf->bInterfaceSubClass);
    console_printf("    Protocol: 0x%02x\r\n", desc_itf->bInterfaceProtocol);
    console_printf("    Endpoints: %u\r\n", desc_itf->bNumEndpoints);

    if (desc_itf->bInterfaceClass == TUSB_CLASS_PRINTER)
      return open_printer_interface(dev_addr, desc_itf, drv_len);

    p_desc += drv_len;
  }

  return 0;
}

static void compute_time_diff(const si_time_t *start, const si_time_t *end,
                              int *out_minutes, uint *out_seconds) {

  if (!start || !end || !out_minutes || !out_seconds)
    return;

  uint half_day_deltas = 0;

  if (start->is_detailed && end->is_detailed) {
    short end_day = end->day_of_week;
    short start_day = start->day_of_week;
    short day_delta = end_day - start_day % 7;
    day_delta = day_delta < 0 ? day_delta + 7 : day_delta;
    half_day_deltas = day_delta * 2;
  } else {
    if (end->seconds_since_midnight < start->seconds_since_midnight)
      half_day_deltas = 1;
  }

  uint seconds = end->seconds_since_midnight - start->seconds_since_midnight;
  seconds += half_day_deltas * 12 * 60 * 60;

  *out_minutes = (seconds / 60);
  *out_seconds = seconds % 60;
}

static void decode_time(const si_time_t *time, uint *out_hours,
                        int *out_minutes, uint *out_seconds) {

  if (!time || !out_hours || !out_minutes || !out_seconds)
    return;

  *out_hours = time->seconds_since_midnight / (60 * 60);
  *out_minutes = (time->seconds_since_midnight / 60) % 60;
  *out_seconds = time->seconds_since_midnight % 60;
}

// Print a card readout. Returns 0 on success.
int printer_print(const si_card_readout_t *readout) {
  uint hours, seconds;
  int minutes;
  bool use_first_punch_as_start = false;

  if (!readout)
    return -1;

  // If the start is not punched, but there are punches, use the first punch as
  // the start.
  if (!readout->start.is_punched && readout->punch_count > 0)
    use_first_punch_as_start = true;

  s_printed = 0;
  write_command(&s_printed, PRINTER_INIT);

  write_command(&s_printed, PRINTER_MODE_SET);
  s_print_buffer[s_printed++] = PRINTER_MODE_DHEIGHT;
  write(&s_printed, "%ld\n", (long int)readout->card_number);

  write_command(&s_printed, PRINTER_MODE_SET);
  s_print_buffer[s_printed++] = 0;

  write(&s_printed, "Bricknr: %ld\n", (long int)readout->card_number);
  write_command(&s_printed, PRINTER_FEED);
  s_print_buffer[s_printed++] = 1; // Feed 1 line.

  write_command(&s_printed, PRINTER_MODE_SET);
  s_print_buffer[s_printed++] = PRINTER_MODE_BOLD | PRINTER_MODE_DHEIGHT;
  write(&s_printed, "Resultat:  ");

  if (use_first_punch_as_start) {
    compute_time_diff(&readout->punches[0].time, &readout->finish.time,
                      &minutes, &seconds);
  } else {
    compute_time_diff(&readout->start.time, &readout->finish.time, &minutes,
                      &seconds);
  }

  write_command(&s_printed, PRINTER_MODE_SET);
  s_print_buffer[s_printed++] =
      PRINTER_MODE_DWIDTH | PRINTER_MODE_DHEIGHT | PRINTER_MODE_BOLD;
  if ((readout->start.is_punched || use_first_punch_as_start) &&
      readout->finish.is_punched) {
    write(&s_printed, "%d:%02d\n", minutes, seconds);
    console_printf("  Result: %d:%02d\r\n", minutes, seconds);
  } else {
    write(&s_printed, "--\n");
    console_printf("  Result: --\r\n");
  }

  write_command(&s_printed, PRINTER_MODE_SET);
  s_print_buffer[s_printed++] = 0;

  write(&s_printed, "Starttid:  ");
  console_printf("  Start time: ");
  if (readout->start.is_punched) {
    decode_time(&readout->start.time, &hours, &minutes, &seconds);
    write(&s_printed, "%02d:%02d:%02d\n", hours, minutes, seconds);
    console_printf("%02d:%02d:%02d\r\n", hours, minutes, seconds);
  } else {
    write(&s_printed, "--\n");
    console_printf("--\r\n");
  }

  write(&s_printed, "M\x86ltid:    ");
  console_printf("  Finish time: ");
  if (readout->finish.is_punched) {
    decode_time(&readout->finish.time, &hours, &minutes, &seconds);
    write(&s_printed, "%02d:%02d:%02d\n", hours, minutes, seconds);
    console_printf("%02d:%02d:%02d\r\n", hours, minutes, seconds);
  } else {
    write(&s_printed, "--\n");
    console_printf("--\r\n");
  }

  write_command(&s_printed, PRINTER_FEED);
  s_print_buffer[s_printed++] = 1;

  write(&s_printed, "%-10s %10s %10s\n", "Str.",
        "Str\x84"
        "cktid",
        "Totaltid");
  write(&s_printed, "--------------------------------\n");

  for (uint i = 0; i < readout->punch_count; i++) {
    char buf[14];
    const si_punch_t *punch = &readout->punches[i];

    snprintf(buf, sizeof(buf), "%2d(%d)", i + 1, (short)punch->station);
    write(&s_printed, "%-10s ", buf);
    console_printf("  %-10s ", buf);

    const si_punch_t *diff_start = &readout->start;
    if (i > 0)
      diff_start = &readout->punches[i - 1];
    if (diff_start->is_punched && punch->is_punched) {
      compute_time_diff(&diff_start->time, &punch->time, &minutes, &seconds);
      snprintf(buf, sizeof(buf), "%+02d:%02d", minutes, seconds);
    } else {
      snprintf(buf, sizeof(buf), "--");
    }
    write(&s_printed, "%10s ", buf);
    console_printf("%10s ", buf);

    if ((readout->start.is_punched || use_first_punch_as_start) &&
        punch->is_punched) {
      if (use_first_punch_as_start) {
        compute_time_diff(&readout->punches[0].time, &punch->time, &minutes,
                          &seconds);
      } else {
        compute_time_diff(&readout->start.time, &punch->time, &minutes,
                          &seconds);
      }
      snprintf(buf, sizeof(buf), "%02d:%02d", minutes, seconds);
      write(&s_printed, "%10s\n", buf);
      console_printf("%10s\r\n", buf);
    } else {
      write(&s_printed, "%10s\n", "--");
      console_printf("%10s\r\n", "--");
    }
  }

  write(&s_printed, "%-10s ", "M\x86l");
  console_printf("  %-10s ", "Finish");

  char buf[14];

  const si_punch_t *diff_start = &readout->start;
  if (readout->punch_count > 0)
    diff_start = &readout->punches[readout->punch_count - 1];

  if (diff_start->is_punched && readout->finish.is_punched) {
    compute_time_diff(&diff_start->time, &readout->finish.time, &minutes,
                      &seconds);
    snprintf(buf, sizeof(buf), "%+02d:%02d", minutes, seconds);
  } else {
    snprintf(buf, sizeof(buf), "--");
  }
  write(&s_printed, "%10s ", buf);
  console_printf("%10s ", buf);

  if ((readout->start.is_punched || use_first_punch_as_start) &&
      readout->finish.is_punched) {
    if (use_first_punch_as_start) {
      compute_time_diff(&readout->punches[0].time, &readout->finish.time,
                        &minutes, &seconds);
    } else {
      compute_time_diff(&readout->start.time, &readout->finish.time, &minutes,
                        &seconds);
    }
    snprintf(buf, sizeof(buf), "%02d:%02d", minutes, seconds);
    write(&s_printed, "%10s\n", buf);
    console_printf("%10s\r\n", buf);
  } else {
    write(&s_printed, "%10s\n", "--");
    console_printf("%10s\r\n", "--");
  }

  write_command(&s_printed, PRINTER_FEED);
  s_print_buffer[s_printed++] = 1;

  // write_command(&s_printed, PRINTER_JUSTIFICATION);
  // s_print_buffer[s_printed++] = PRINTER_JUSTIFICATION_CENTER;
  // write(&s_printed, "Final text here.\n");

  write_command(&s_printed, PRINTER_FEED);
  s_print_buffer[s_printed++] = 2;

  memset(&s_print_buffer[s_printed], 0, sizeof(s_print_buffer) - s_printed);

  if (!s_initialized)
    return -1;

  s_xfer.user_data = USB_FRAME_SIZE;
  s_xfer.buffer = s_print_buffer;
  s_xfer.buflen = USB_FRAME_SIZE;
  tuh_edpt_xfer(&s_xfer);

  return 0;
}

void printer_xfer_callback(tuh_xfer_t *xfer) {
  if (xfer->result != XFER_RESULT_SUCCESS)
    return;

  int sent = xfer->user_data;

  if (sent < s_printed) {
    xfer->buffer = &s_print_buffer[sent];
    xfer->buflen = USB_FRAME_SIZE;
    xfer->user_data = sent + USB_FRAME_SIZE;
    tuh_edpt_xfer(xfer);
  } else {
    if (s_done_cb)
      s_done_cb();
  }
}

// Register a callback to be called when the printer is done printing.
void printer_register_done_cb(void (*cb)(void)) { s_done_cb = cb; }

//
// STATIC FUNCITONS
//

static uint16_t count_interface_total_len(const tusb_desc_interface_t *desc_itf,
                                          uint8_t itf_count, uint16_t max_len) {

  const uint8_t *p_desc = (const uint8_t *)desc_itf;
  uint16_t len = 0;

  while (itf_count--) {
    len += tu_desc_len(p_desc);
    p_desc = tu_desc_next(p_desc);

    while (len < max_len) {
      if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE_ASSOCIATION)
        return len;

      if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE &&
          ((const tusb_desc_interface_t *)p_desc)->bAlternateSetting == 0)
        break;

      len += tu_desc_len(p_desc);
      p_desc = tu_desc_next(p_desc);
    }
  }

  return len;
}

static int open_printer_interface(uint8_t dev_addr,
                                  const tusb_desc_interface_t *desc_itf,
                                  uint16_t max_len) {
  const uint16_t drv_len =
      sizeof(tusb_desc_interface_t) +
      desc_itf->bNumEndpoints * sizeof(tusb_desc_endpoint_t);

  if (drv_len > max_len)
    return -1;

  const uint8_t *p_desc = (const uint8_t *)desc_itf;

  p_desc = tu_desc_next(p_desc);
  const tusb_desc_endpoint_t *desc_ep = (const tusb_desc_endpoint_t *)p_desc;

  for (int i = 0; i < desc_itf->bNumEndpoints; i++) {
    if (desc_ep->bDescriptorType != TUSB_DESC_ENDPOINT)
      return -1;

    if (tu_edpt_dir(desc_ep->bEndpointAddress) == TUSB_DIR_OUT) {
      s_initialized = tuh_edpt_open(dev_addr, desc_ep);
      s_xfer.daddr = dev_addr;
      s_xfer.ep_addr = desc_ep->bEndpointAddress;
      s_xfer.complete_cb = printer_xfer_callback;
      s_xfer.buflen = USB_FRAME_SIZE;

      return s_initialized ? 0 : -1;
    }

    p_desc = tu_desc_next(p_desc);
    desc_ep = (const tusb_desc_endpoint_t *)p_desc;
  }

  return -1;
}

static inline void write_command(int *offset, uint16_t command) {
  if (*offset + 2 >= (int)sizeof(s_print_buffer))
    return;

  s_print_buffer[(*offset)++] = command >> 8;
  s_print_buffer[(*offset)++] = command & 0xFF;
}

static void write(int *offset, const char *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  *offset += vsnprintf((char *)&s_print_buffer[*offset],
                       sizeof(s_print_buffer) - *offset, format, argptr);
  va_end(argptr);
}
