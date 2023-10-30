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

#include "console.h"

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

int main(void) {

  set_sys_clock_khz(120000, true);

  sleep_ms(10);

  multicore_reset_core1();
  multicore_launch_core1(core1_main);

  console_init();

  while (true) {
    console_tick();
    sleep_ms(1);
  }

  return 0;
}

void tuh_mount_cb(uint8_t daddr) {
  console_printf("A device with address %d is mounted\r\n", daddr);
}

void tuh_umount_cb(uint8_t daddr) {
  console_printf("A device with address %d is unmounted\r\n", daddr);
}

void tuh_cdc_mount_cb(uint8_t idx) {
  console_printf("tud_cdc_mount_cb\r\n");
  tuh_itf_info_t itf_info = {0};
  tuh_cdc_itf_get_info(idx, &itf_info);

  console_printf("CDC Interface is mounted: address = %u, itf_num = %u\r\n",
                 itf_info.daddr, itf_info.desc.bInterfaceNumber);

  tuh_cdc_set_baudrate(idx, 38400, NULL, 0);
  tuh_cdc_set_control_line_state(idx, 0b11, NULL, 0);
}

void tuh_cdc_umount_cb(uint8_t idx) {
  console_printf("tud_cdc_umount_cb\r\n");
  tuh_itf_info_t itf_info = {0};
  tuh_cdc_itf_get_info(idx, &itf_info);

  console_printf("CDC Interface is unmounted: address = %u, itf_num = %u\r\n",
                 itf_info.daddr, itf_info.desc.bInterfaceNumber);
}

void tuh_cdc_rx_cb(uint8_t idx) {
  uint8_t buf[64 + 1]; // +1 for extra null character
  uint32_t const bufsize = sizeof(buf) - 1;

  // forward cdc interfaces -> console
  uint32_t count = tuh_cdc_read(idx, buf, bufsize);
  buf[count] = 0;

  console_printf("Received %u bytes: ", (unsigned int)count);
  for (uint i = 0; i < count; i++)
    console_printf("%02x", buf[i]);

  console_printf("\r\n");

  uint8_t obuf[2] = {0xff, 0x06};
  if (buf[1] == 0xe5) {
    tuh_cdc_write(idx, obuf, 2);
    tuh_cdc_write_flush(idx);
  }
}
