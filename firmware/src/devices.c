#include <stdbool.h>
#include <stdint.h>
#include <tusb.h>

#include <pio_usb.h>

#include "console.h"
#include "devices.h"

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

void tuh_cdc_mount_cb(uint8_t idx) {
  // Set the baudrate of the SI reader station
  tuh_cdc_set_baudrate(idx, 38400, NULL, 0);

  // Not sure if this is needed.
  // tuh_cdc_set_control_line_state(idx, 0b11, NULL, 0);
}

void tuh_cdc_rx_cb(uint8_t idx) {
  uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];
  unsigned int bufsize = sizeof(buf);

  if (!tuh_cdc_mounted(idx) || tuh_cdc_read_available(idx) == 0)
    return;

  // Read received data
  uint32_t count = tuh_cdc_read(idx, buf, bufsize);
  console_printf("Received %u bytes: ", (unsigned int)count);

  if (input_callback != NULL)
    input_callback(buf, count);
}
