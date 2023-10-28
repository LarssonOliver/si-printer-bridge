#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "host/usbh.h"
#include "pico/stdlib.h"

#include "pio_usb.h"
#include "tusb.h"

// REF:
// https://github.dev/sekigon-gonnoc/Pico-PIO-USB/blob/main/examples/host_hid_to_device_cdc
// https://github.com/hathach/tinyusb/blob/master/examples/host/cdc_msc_hid/src/main.c

int main(void) {

  set_sys_clock_khz(120000, true);

  pio_usb_configuration_t pio_config = PIO_USB_DEFAULT_CONFIG;
  tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION,
                &pio_config);

  tuh_init(BOARD_TUH_RHPORT);

  while (true) {
    tuh_task();
  }

  return 0;
}

void tuh_mount_cb(uint8_t daddr) {
  printf("A device with address %d is mounted\r\n", daddr);
}

void tuh_umount_cb(uint8_t daddr) {
  printf("A device with address %d is unmounted\r\n", daddr);
}
