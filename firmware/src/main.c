/**
 * Copyright (C) 2024 Oliver Larsson
 *
 * This file is part of si-printer.
 *
 * si-printer is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * si-printer is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * si-printer. If not, see <https://www.gnu.org/licenses/>.
 */

#include <hardware/watchdog.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <tusb.h>

#include "console.h"
#include "data_processor.h"
#include "devices.h"

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

  watchdog_enable(1000, true);

  devices_init();
  devices_register_input_callback(on_data_received);

  bool second_init = false;

  while (true) {
    // This core does most of the work.
    watchdog_update();

    devices_tick();

    if (!second_init) {
      absolute_time_t time = get_absolute_time();
      if (to_ms_since_boot(time) > 1000) {
        devices_init2();
        second_init = true;
      }
    }
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

    if (!console_is_connected())
      sleep_ms(10);
  }

  return 0;
}
