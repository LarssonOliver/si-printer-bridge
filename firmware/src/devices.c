/**
 * Copyright (C) 2024 Oliver Larsson
 *
 * This file is part of si-printer-bridge.
 *
 * si-printer-bridge is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * si-printer-bridge is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * si-printer-bridge. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>
#include <tusb.h>

#include <pio_usb.h>

#include "console.h"
#include "devices.h"
#include "printer.h"

static input_callback_t input_callback = NULL;

static uint8_t s_reader_id = 0xFF;

void devices_init(void) {
  pio_usb_configuration_t pio_config = PIO_USB_DEFAULT_CONFIG;
  tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION,
                &pio_config);

  tuh_init(BOARD_TUH_RHPORT);
}

void devices_init2(void) { pio_usb_host_add_port(8, PIO_USB_PINOUT_DPDM); }

void devices_tick(void) { tuh_task(); }

void devices_register_input_callback(input_callback_t cb) {
  input_callback = cb;
}

uint32_t devices_reader_write(const void *buf, uint32_t size) {
  if (s_reader_id == 0xFF)
    return 0;

  uint32_t written = tuh_cdc_write(s_reader_id, buf, size);
  tuh_cdc_write_flush(s_reader_id);

  console_printf("Wrote    %u bytes: ", (unsigned int)written);
  for (uint i = 0; i < size; i++)
    console_printf("%02x", ((uint8_t *)buf)[i]);

  console_printf("\r\n");

  return written;
}

//
// -- TUSB Callbacks --
//

// -- RAW --

void tuh_mount_cb(uint8_t dev_addr) {
  console_printf("Device %u mounted\r\n", dev_addr);

  tusb_desc_device_t desc_device;
  if (tuh_descriptor_get_device_sync(
          dev_addr, &desc_device, sizeof(desc_device)) != XFER_RESULT_SUCCESS) {
    console_printf("Failed to get device descriptor\r\n");
    return;
  }

  console_printf("  VID: 0x%04x\r\n", desc_device.idVendor);
  console_printf("  PID: 0x%04x\r\n", desc_device.idProduct);

  if (desc_device.idVendor == SI_PRINTER_VID &&
      desc_device.idProduct == SI_PRINTER_PID) {

    if (printer_init(dev_addr) != 0) {
      console_printf("Failed to initialize printer\r\n");
      // Handle this?
      return;
    }
  }
}

// -- CDC --

void tuh_cdc_mount_cb(uint8_t idx) {
  console_printf("CDC Device %u mounted\r\n", idx);

  // Set the baudrate of the SI reader station
  tuh_cdc_set_baudrate(idx, 38400, NULL, 0);

  // Not sure if this is needed.
  tuh_cdc_set_control_line_state(idx, 0b11, NULL, 0);

  s_reader_id = idx;
}

void tuh_cdc_umount_cb(uint8_t idx) {
  (void)idx;
  s_reader_id = 0xFF;
}

void tuh_cdc_rx_cb(uint8_t idx) {
  uint8_t buf[CFG_TUD_CDC_EP_BUFSIZE];
  unsigned int bufsize = sizeof(buf);

  if (!tuh_cdc_mounted(idx) || tuh_cdc_read_available(idx) == 0)
    return;

  // Read received data
  uint32_t count = tuh_cdc_read(idx, buf, bufsize);
  console_printf("Received %u bytes: ", (unsigned int)count);
  for (uint i = 0; i < count; i++)
    console_printf("%02x", buf[i]);
  console_printf("\r\n");

  if (input_callback != NULL)
    input_callback(buf, count);
}
