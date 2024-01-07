#include <tusb.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include "console.h"
#include "devices.h"
#include "data_processor.h"

// REF:
// https://github.dev/sekigon-gonnoc/Pico-PIO-USB/blob/main/examples/host_hid_to_device_cdc
// https://github.com/hathach/tinyusb/blob/master/examples/host/cdc_msc_hid/src/main.c

// TODO: Watchdog stuff.
// TODO: Check if station is in readout mode on connect.
// TODO: Check if connected usb device is a (correct) SI station.

void core1_main(void) {
  sleep_ms(10);

  devices_init();
  devices_register_input_callback(on_data_received);

  while (true) {
    devices_tick();

    if (tuh_cdc_mounted(0) && tuh_cdc_read_available(0) > 0)
      console_printf("%u bytes available\r\n", (unsigned int)tuh_cdc_read_available(0));

    sleep_ms(1);
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
