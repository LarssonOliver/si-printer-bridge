#include <tusb.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <hardware/watchdog.h>

#include "console.h"
#include "devices.h"
#include "data_processor.h"

// REF:
// https://github.dev/sekigon-gonnoc/Pico-PIO-USB/blob/main/examples/host_hid_to_device_cdc
// https://github.com/hathach/tinyusb/blob/master/examples/host/cdc_msc_hid/src/main.c

// TODO: Watchdog stuff.
// TODO: Check if station is in readout mode on connect.
// TODO: Check if connected usb device is a (correct) SI station.
// TODO: Check that station is in extended command mode.
// TODO: Check BAUD rates?

void core1_main(void) {
  sleep_ms(10);

  devices_init();
  devices_register_input_callback(on_data_received);

  watchdog_enable(1000, true);

  while (true) {
    // This core does most of the work.
    watchdog_update();

    devices_tick();

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
    sleep_ms(10);
  }

  return 0;
}
