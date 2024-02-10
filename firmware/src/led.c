/**
 * Copyright (C) 2024 Oliver Larsson
 *
 * This file is part of si-printer-bridge.
 *
 * si-printer-bridge is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * si-printer-bridge is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * si-printer-bridge. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>

#include <pico/stdlib.h>

#include "led.h"

static const uint8_t s_led_pins[] = {LED_INTERNAL_PIN, LED_POWER_PIN,
                                     LED_INFO_PIN};

static bool s_initialized = false;

// Initialize LED driver.
void led_init(void) {
  uint32_t mask = 0;
  for (uint8_t i = 0; i < sizeof(s_led_pins) / sizeof(s_led_pins[0]); i++) {
    mask |= 1 << s_led_pins[i];
  }

  gpio_init_mask(mask);
  gpio_set_dir_out_masked(mask);

  s_initialized = true;
}

// Set LED on or off.
void led_set(LED led, bool on) {
  if (!s_initialized || led >= sizeof(s_led_pins) / sizeof(s_led_pins[0])) {
    return;
  }

  gpio_put(s_led_pins[led], on);
}
