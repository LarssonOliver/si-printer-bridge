#include <stdbool.h>
#include <stdint.h>
#include <tusb.h>

#include <pio_usb.h>

#include "console.h"
#include "devices.h"

void devices_init(void) {
  pio_usb_configuration_t pio_config = PIO_USB_DEFAULT_CONFIG;
  tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION,
                &pio_config);

  tuh_init(BOARD_TUH_RHPORT);
}

void devices_tick(void) { tuh_task(); }

uint32_t devices_write(uint8_t idx, const void *buf, uint32_t size) {
  uint32_t written = tuh_cdc_write(idx, buf, size);
  tuh_cdc_write_flush(idx);

  console_printf("%u: Wrote %u bytes: ", idx, (unsigned int)written);
  for (uint i = 0; i < size; i++)
    console_printf("%02x", ((uint8_t *)buf)[i]);

  console_printf("\r\n");

  return written;
}

void read(void) {
  console_printf("Reading...\r\n");

  uint8_t out[] = {0xff, 0x02, 0xb1, 0x00, 0xb1, 0x00, 0x03};
  devices_write(0, out, sizeof(out));
}

/**
 *  _____ _   _ ____  ____     ____      _ _ _                _
 * |_   _| | | / ___|| __ )   / ___|__ _| | | |__   __ _  ___| | _____
 *   | | | | | \___ \|  _ \  | |   / _` | | | '_ \ / _` |/ __| |/ / __|
 *   | | | |_| |___) | |_) | | |__| (_| | | | |_) | (_| | (__|   <\__ \
 *   |_|  \___/|____/|____/   \____\__,_|_|_|_.__/ \__,_|\___|_|\_\___/
 */

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

  console_printf("%u: Received %u bytes: ", idx, (unsigned int)count);
  for (uint i = 0; i < count; i++)
    console_printf("%02x", buf[i]);

  console_printf("\r\n");

  if (count > 4 && buf[0] == 0x02 && buf[1] == 0xe5 && buf[2] == 0x06 &&
      buf[3] == 0x00)
    read();
}
