#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"

#include "bsp/board.h"
#include "pio_usb.h"
#include "tusb.h"

// REF:
// https://github.dev/sekigon-gonnoc/Pico-PIO-USB/blob/main/examples/host_hid_to_device_cdc
// https://github.com/hathach/tinyusb/blob/master/examples/host/cdc_msc_hid/src/main.c

void cdc_app_task(void);

int main(void) {

  set_sys_clock_khz(120000, true);

  /* pio_usb_configuration_t pio_config = PIO_USB_DEFAULT_CONFIG; */
  /* tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, */
  /*               &pio_config); */

  tud_init(BOARD_TUD_RHPORT);
  /* tuh_init(BOARD_TUH_RHPORT); */

  uint counter = 0;

  while (true) {
    tud_task();

    if (++counter > 500) {
      tud_cdc_write_str("Hello\n");
      counter -= 500;
    }

    tud_cdc_write_flush();

    /* tuh_task(); */
    /* cdc_app_task(); */

    sleep_ms(1);
  }

  return 0;
}

void tuh_mount_cb(uint8_t daddr) {
  printf("A device with address %d is mounted\r\n", daddr);
}

void tuh_umount_cb(uint8_t daddr) {
  printf("A device with address %d is unmounted\r\n", daddr);
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
  tuh_cdc_itf_info_t itf_info = {0};
  tuh_cdc_itf_get_info(idx, &itf_info);

  printf("CDC Interface is mounted: address = %u, itf_num = %u\r\n",
         itf_info.daddr, itf_info.bInterfaceNumber);

  const cdc_line_coding_t line_coding = {38400, CDC_LINE_CONDING_STOP_BITS_1,
                                         CDC_LINE_CODING_PARITY_NONE, 8};

  tuh_cdc_set_line_coding(idx, &line_coding, cdc_cb, (uintptr_t)NULL);
}

void tuh_cdc_umount_cb(uint8_t idx) {
  tuh_cdc_itf_info_t itf_info = {0};
  tuh_cdc_itf_get_info(idx, &itf_info);

  printf("CDC Interface is unmounted: address = %u, itf_num = %u\r\n",
         itf_info.daddr, itf_info.bInterfaceNumber);
}

void cdc_cb(tuh_xfer_t *xfer) {
  printf("xfer cb triggered: %u\n", xfer->daddr);
}

void tud_cdc_rx_cb(uint8_t itf)
{
  (void) itf;

  char buf[64];
  uint32_t count = tud_cdc_read(buf, sizeof(buf));

  // TODO control LED on keyboard of host stack
  (void) count;
}
