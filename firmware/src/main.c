#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "host/usbh.h"
#include "pico/stdlib.h"

#include "pio_usb.h"
#include "tusb.h"
#include "bsp/board.h"

// REF:
// https://github.dev/sekigon-gonnoc/Pico-PIO-USB/blob/main/examples/host_hid_to_device_cdc
// https://github.com/hathach/tinyusb/blob/master/examples/host/cdc_msc_hid/src/main.c

void cdc_app_task(void);

int main(void) {

  set_sys_clock_khz(120000, true);

  pio_usb_configuration_t pio_config = PIO_USB_DEFAULT_CONFIG;
  tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION,
                &pio_config);

  tuh_init(BOARD_TUH_RHPORT);

  while (true) {
    tuh_task();
    cdc_app_task();
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

void tuh_cdc_mount_cb(uint8_t idx) {
  tuh_cdc_itf_info_t itf_info = {0};
  tuh_cdc_itf_get_info(idx, &itf_info);

  printf("CDC Interface is mounted: address = %u, itf_num = %u\r\n",
         itf_info.daddr, itf_info.bInterfaceNumber);

#ifdef CFG_TUH_CDC_LINE_CODING_ON_ENUM
  // CFG_TUH_CDC_LINE_CODING_ON_ENUM must be defined for line coding is set by
  // tinyusb in enumeration otherwise you need to call tuh_cdc_set_line_coding()
  // first
  cdc_line_coding_t line_coding = {0};
  if (tuh_cdc_get_local_line_coding(idx, &line_coding)) {
    printf("  Baudrate: %lu, Stop Bits : %u\r\n", line_coding.bit_rate,
           line_coding.stop_bits);
    printf("  Parity  : %u, Data Width: %u\r\n", line_coding.parity,
           line_coding.data_bits);
  }
#endif
}

void tuh_cdc_umount_cb(uint8_t idx) {
  tuh_cdc_itf_info_t itf_info = {0};
  tuh_cdc_itf_get_info(idx, &itf_info);

  printf("CDC Interface is unmounted: address = %u, itf_num = %u\r\n",
         itf_info.daddr, itf_info.bInterfaceNumber);
}
