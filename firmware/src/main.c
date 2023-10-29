#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "bsp/board_api.h"
#include "pio_usb.h"
#include "tusb.h"

// REF:
// https://github.dev/sekigon-gonnoc/Pico-PIO-USB/blob/main/examples/host_hid_to_device_cdc
// https://github.com/hathach/tinyusb/blob/master/examples/host/cdc_msc_hid/src/main.c

void cdc_app_task(void);


void core1_main(void) {
  sleep_ms(10);

  pio_usb_configuration_t pio_config = PIO_USB_DEFAULT_CONFIG;
  tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION,
                &pio_config);

  tuh_init(BOARD_TUH_RHPORT);

  while (true) {
    tuh_task();
    /* cdc_app_task(); */
  }
}

int cdc_printf(const char *__restrict, ...)
    _ATTRIBUTE((__format__(__printf__, 1, 2)));

int cdc_printf(const char *format, ...) {
  va_list argptr;
  va_start(argptr, format);

  char buf[256];
  /* snprintf(buf, 256, format); */
  vsprintf(buf, format, argptr);
  va_end(argptr);

  return tud_cdc_write_str(buf);
}

int main(void) {

  /* set_sys_clock_khz(120000, true); */

  sleep_ms(10);

  multicore_reset_core1();
  multicore_launch_core1(core1_main);

  tud_init(BOARD_TUD_RHPORT);

  /* unsigned int counter = 0; */

  while (true) {
    /* if (++counter > 5000) { */
    /*   cdc_printf("Ping\r\n"); */
    /*   counter -= 5000; */
    /* } */

    tud_task();
    tud_cdc_write_flush();

    sleep_ms(1);
  }

  return 0;
}

void tuh_xfer_cb(tuh_xfer_t *data) { cdc_printf("cb: %d\r\n", data->daddr); }

tusb_desc_device_t desc_device_h;
void print_device_descriptor(tuh_xfer_t *xfer);
#define LANGUAGE_ID 0x0409
void parse_config_descriptor(uint8_t dev_addr,
                             tusb_desc_configuration_t const *desc_cfg);

void tuh_mount_cb(uint8_t daddr) {
  cdc_printf("A device with address %d is mounted\r\n", daddr);
  tuh_descriptor_get_device(daddr, &desc_device_h, 18, print_device_descriptor,
                            0);
}

static void _convert_utf16le_to_utf8(const uint16_t *utf16, size_t utf16_len,
                                     uint8_t *utf8, size_t utf8_len) {
  // TODO: Check for runover.
  (void)utf8_len;
  // Get the UTF-16 length out of the data itself.

  for (size_t i = 0; i < utf16_len; i++) {
    uint16_t chr = utf16[i];
    if (chr < 0x80) {
      *utf8++ = chr & 0xffu;
    } else if (chr < 0x800) {
      *utf8++ = (uint8_t)(0xC0 | (chr >> 6 & 0x1F));
      *utf8++ = (uint8_t)(0x80 | (chr >> 0 & 0x3F));
    } else {
      // TODO: Verify surrogate.
      *utf8++ = (uint8_t)(0xE0 | (chr >> 12 & 0x0F));
      *utf8++ = (uint8_t)(0x80 | (chr >> 6 & 0x3F));
      *utf8++ = (uint8_t)(0x80 | (chr >> 0 & 0x3F));
    }
    // TODO: Handle UTF-16 code points that take two entries.
  }
}

// Count how many bytes a utf-16-le encoded string will take in utf-8.
static int _count_utf8_bytes(const uint16_t *buf, size_t len) {
  size_t total_bytes = 0;
  for (size_t i = 0; i < len; i++) {
    uint16_t chr = buf[i];
    if (chr < 0x80) {
      total_bytes += 1;
    } else if (chr < 0x800) {
      total_bytes += 2;
    } else {
      total_bytes += 3;
    }
    // TODO: Handle UTF-16 code points that take two entries.
  }
  return (int)total_bytes;
}

static void print_utf16(uint16_t *temp_buf, size_t buf_len) {
  if ((temp_buf[0] & 0xff) == 0)
    return; // empty
  size_t utf16_len = ((temp_buf[0] & 0xff) - 2) / sizeof(uint16_t);
  size_t utf8_len = (size_t)_count_utf8_bytes(temp_buf + 1, utf16_len);
  _convert_utf16le_to_utf8(temp_buf + 1, utf16_len, (uint8_t *)temp_buf,
                           sizeof(uint16_t) * buf_len);
  ((uint8_t *)temp_buf)[utf8_len] = '\0';

  cdc_printf("%s", (char *)temp_buf);
}

void print_device_descriptor(tuh_xfer_t *xfer) {
  if (XFER_RESULT_SUCCESS != xfer->result) {
    cdc_printf("Failed to get device descriptor\r\n");
    return;
  }

  uint8_t const daddr = xfer->daddr;

  cdc_printf("Device %u: ID %04x:%04x\r\n", daddr, desc_device_h.idVendor,
             desc_device_h.idProduct);
  cdc_printf("Device Descriptor:\r\n");
  cdc_printf("  bLength             %u\r\n", desc_device_h.bLength);
  cdc_printf("  bDescriptorType     %u\r\n", desc_device_h.bDescriptorType);
  cdc_printf("  bcdUSB              %04x\r\n", desc_device_h.bcdUSB);
  cdc_printf("  bDeviceClass        %u\r\n", desc_device_h.bDeviceClass);
  cdc_printf("  bDeviceSubClass     %u\r\n", desc_device_h.bDeviceSubClass);
  cdc_printf("  bDeviceProtocol     %u\r\n", desc_device_h.bDeviceProtocol);
  cdc_printf("  bMaxPacketSize0     %u\r\n", desc_device_h.bMaxPacketSize0);
  cdc_printf("  idVendor            0x%04x\r\n", desc_device_h.idVendor);
  cdc_printf("  idProduct           0x%04x\r\n", desc_device_h.idProduct);
  cdc_printf("  bcdDevice           %04x\r\n", desc_device_h.bcdDevice);

  // Get String descriptor using Sync API
  uint16_t temp_buf[128];

  cdc_printf("  iManufacturer       %u     ", desc_device_h.iManufacturer);
  if (XFER_RESULT_SUCCESS ==
      tuh_descriptor_get_manufacturer_string_sync(daddr, LANGUAGE_ID, temp_buf,
                                                  sizeof(temp_buf))) {
    print_utf16(temp_buf, TU_ARRAY_SIZE(temp_buf));
  }
  cdc_printf("\r\n");

  cdc_printf("  iProduct            %u     ", desc_device_h.iProduct);
  if (XFER_RESULT_SUCCESS ==
      tuh_descriptor_get_product_string_sync(daddr, LANGUAGE_ID, temp_buf,
                                             sizeof(temp_buf))) {
    print_utf16(temp_buf, TU_ARRAY_SIZE(temp_buf));
  }
  cdc_printf("\r\n");

  cdc_printf("  iSerialNumber       %u     ", desc_device_h.iSerialNumber);
  if (XFER_RESULT_SUCCESS ==
      tuh_descriptor_get_serial_string_sync(daddr, LANGUAGE_ID, temp_buf,
                                            sizeof(temp_buf))) {
    print_utf16(temp_buf, TU_ARRAY_SIZE(temp_buf));
  }
  cdc_printf("\r\n");

  cdc_printf("  bNumConfigurations  %u\r\n", desc_device_h.bNumConfigurations);

  // Get configuration descriptor with sync API
  if (XFER_RESULT_SUCCESS == tuh_descriptor_get_configuration_sync(
                                 daddr, 0, temp_buf, sizeof(temp_buf))) {
    parse_config_descriptor(daddr, (tusb_desc_configuration_t *)temp_buf);
  }
}

// count total length of an interface
uint16_t count_interface_total_len(tusb_desc_interface_t const *desc_itf,
                                   uint8_t itf_count, uint16_t max_len);

/* void open_hid_interface(uint8_t daddr, tusb_desc_interface_t const *desc_itf,
 */
/*                         uint16_t max_len); */

// simple configuration parser to open and listen to HID Endpoint IN
void parse_config_descriptor(uint8_t __attribute__((unused)) dev_addr,
                             tusb_desc_configuration_t const *desc_cfg) {
  uint8_t const *desc_end =
      ((uint8_t const *)desc_cfg) + tu_le16toh(desc_cfg->wTotalLength);
  uint8_t const *p_desc = tu_desc_next(desc_cfg);

  // parse each interfaces
  while (p_desc < desc_end) {
    uint8_t assoc_itf_count = 1;

    // Class will always starts with Interface Association (if any) and then
    // Interface descriptor
    if (TUSB_DESC_INTERFACE_ASSOCIATION == tu_desc_type(p_desc)) {
      tusb_desc_interface_assoc_t const *desc_iad =
          (tusb_desc_interface_assoc_t const *)p_desc;
      assoc_itf_count = desc_iad->bInterfaceCount;

      p_desc = tu_desc_next(p_desc); // next to Interface
    }

    // must be interface from now
    if (TUSB_DESC_INTERFACE != tu_desc_type(p_desc))
      return;
    tusb_desc_interface_t const *desc_itf =
        (tusb_desc_interface_t const *)p_desc;

    uint16_t const drv_len = count_interface_total_len(
        desc_itf, assoc_itf_count, (uint16_t)(desc_end - p_desc));

    cdc_printf("Interface size: %d\r\n", drv_len);
    cdc_printf("  interface: %d\r\n", desc_itf->iInterface);
    cdc_printf("  class: %d\r\n", desc_itf->bInterfaceClass);
    cdc_printf("  endpoints: %d\r\n", desc_itf->bNumEndpoints);
    cdc_printf("  descriptor type: %d\r\n", desc_itf->bDescriptorType);
    cdc_printf("  subclass: %d\r\n", desc_itf->bInterfaceSubClass);
    cdc_printf("  number: %d\r\n", desc_itf->bInterfaceNumber);
    cdc_printf("  protocol: %d\r\n", desc_itf->bInterfaceProtocol);

    // probably corrupted descriptor
    if (drv_len < sizeof(tusb_desc_interface_t))
      return;

    // only open and listen to HID endpoint IN
    if (desc_itf->bInterfaceClass == TUSB_CLASS_HID) {
      cdc_printf("HID?\r\n");
    }

    // next Interface or IAD descriptor
    p_desc += drv_len;
  }
}

uint16_t count_interface_total_len(tusb_desc_interface_t const *desc_itf,
                                   uint8_t itf_count, uint16_t max_len) {
  uint8_t const *p_desc = (uint8_t const *)desc_itf;
  uint16_t len = 0;

  while (itf_count--) {
    // Next on interface desc
    len += tu_desc_len(desc_itf);
    p_desc = tu_desc_next(p_desc);

    while (len < max_len) {
      // return on IAD regardless of itf count
      if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE_ASSOCIATION)
        return len;

      if ((tu_desc_type(p_desc) == TUSB_DESC_INTERFACE) &&
          ((tusb_desc_interface_t const *)p_desc)->bAlternateSetting == 0) {
        break;
      }

      len += tu_desc_len(p_desc);
      p_desc = tu_desc_next(p_desc);
    }
  }

  return len;
}

void tuh_umount_cb(uint8_t daddr) {
  cdc_printf("A device with address %d is unmounted\r\n", daddr);
}

size_t get_console_inputs(uint8_t *buf, size_t bufsize) {
  size_t count = 0;
  while (count < bufsize) {
    int ch = board_getchar();
    if (ch <= 0)
      break;

    buf[count] = (uint8_t)ch;
    count++;
  }

  return count;
}

void cdc_app_task(void) {
  uint8_t buf[64 + 1]; // +1 for extra null character
  uint32_t const bufsize = sizeof(buf) - 1;

  uint32_t count = get_console_inputs(buf, bufsize);
  buf[count] = 0;

  // loop over all mounted interfaces
  for (uint8_t idx = 0; idx < CFG_TUH_CDC; idx++) {
    if (tuh_cdc_mounted(idx)) {
      cdc_printf("cdc_mounted\r\n");
      // console --> cdc interfaces
      if (count) {
        tuh_cdc_write(idx, buf, count);
        tuh_cdc_write_flush(idx);
      }
    }
  }
}

void cdc_cb(tuh_xfer_t *xfer);

void tuh_cdc_mount_cb(uint8_t idx) {
  cdc_printf("tud_cdc_mount_cb\r\n");
  tuh_itf_info_t itf_info = {0};
  tuh_cdc_itf_get_info(idx, &itf_info);

  cdc_printf("CDC Interface is mounted: address = %u, itf_num = %u\r\n",
             itf_info.daddr, itf_info.desc.bInterfaceNumber);

  tuh_cdc_set_baudrate(idx, 38400, NULL, 0);
  tuh_cdc_set_control_line_state(idx, 0b11, NULL, 0);

  /* #endif */

  /* const cdc_line_coding_t line_coding = {38400, CDC_LINE_CONDING_STOP_BITS_1,
   */
  /*                                        CDC_LINE_CODING_PARITY_NONE, 8}; */

  /* tuh_cdc_set_line_coding(idx, &line_coding, cdc_cb, (uintptr_t)NULL); */
}

void tuh_cdc_umount_cb(uint8_t idx) {
  cdc_printf("tud_cdc_umount_cb\r\n");
  tuh_itf_info_t itf_info = {0};
  tuh_cdc_itf_get_info(idx, &itf_info);

  cdc_printf("CDC Interface is unmounted: address = %u, itf_num = %u\r\n",
             itf_info.daddr, itf_info.desc.bInterfaceNumber);
}

void cdc_cb(tuh_xfer_t *xfer) {
  cdc_printf("xfer cb triggered: %u\r\n", xfer->daddr);
}

void tud_cdc_rx_cb(uint8_t itf) {
  cdc_printf("tud_cdc_rx_cb\r\n");
  (void)itf;

  char buf[64];
  uint32_t count = tud_cdc_read(buf, sizeof(buf));

  for (uint i = 0; i < count; i++)
    cdc_printf("%x", buf[i]);

  cdc_printf("\r\n");

  // TODO control LED on keyboard of host stack
  (void)count;
}

void tuh_cdc_rx_cb(uint8_t idx) {
  uint8_t buf[64 + 1]; // +1 for extra null character
  uint32_t const bufsize = sizeof(buf) - 1;

  // forward cdc interfaces -> console
  uint32_t count = tuh_cdc_read(idx, buf, bufsize);
  buf[count] = 0;

  cdc_printf("Received %u bytes: ", (unsigned int)count);
  for (uint i = 0; i < count; i++)
    cdc_printf("%02x", buf[i]);

  cdc_printf("\r\n");

  uint8_t obuf[2] = { 0xff, 0x06 };
  if (buf[1] == 0xe5) {
    tuh_cdc_write(idx, obuf, 2);
    tuh_cdc_write_flush(idx);
  }
}
