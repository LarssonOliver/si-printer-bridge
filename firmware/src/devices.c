#include <stdbool.h>
#include <stdint.h>
#include <tusb.h>

#include <pio_usb.h>

#include "console.h"
#include "devices.h"
#include "printer.h"

static input_callback_t input_callback = NULL;

void devices_init(void) {
  pio_usb_configuration_t pio_config = PIO_USB_DEFAULT_CONFIG;
  tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION,
                &pio_config);

  tuh_init(BOARD_TUH_RHPORT);
}

void devices_tick(void) { tuh_task(); }

void devices_register_input_callback(input_callback_t cb) {
  input_callback = cb;
}

uint32_t devices_write(uint8_t idx, const void *buf, uint32_t size) {
  uint32_t written = tuh_cdc_write(idx, buf, size);
  tuh_cdc_write_flush(idx);

  console_printf("%u: Wrote %u bytes: ", idx, (unsigned int)written);
  for (uint i = 0; i < size; i++)
    console_printf("%02x", ((uint8_t *)buf)[i]);

  console_printf("\r\n");

  return written;
}

//
// -- TUSB Callbacks --
//

// -- CDC --

void tuh_cdc_mount_cb(uint8_t idx) {
  // Set the baudrate of the SI reader station
  tuh_cdc_set_baudrate(idx, 38400, NULL, 0);

  // Not sure if this is needed.
  // tuh_cdc_set_control_line_state(idx, 0b11, NULL, 0);
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

  // console_printf("  VID: %04x\r\n", desc_device.idVendor);
  // console_printf("  PID: %04x\r\n", desc_device.idProduct);
  // console_printf("  PID: %04x\r\n", desc_device.);
}
