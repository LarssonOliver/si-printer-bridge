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

#pragma once

#include <stdbool.h>

#ifndef LED_INTERNAL_PIN
#define LED_INTERNAL_PIN 25
#endif

#ifndef LED_POWER_PIN
#define LED_POWER_PIN 4
#endif

#ifndef LED_INFO_PIN
#define LED_INFO_PIN 5
#endif

typedef enum {
  LED_INTERNAL = 0,
  LED_POWER,
  LED_INFO,
} LED;

// Initialize LED driver.
void led_init(void);

// Set LED on or off.
void led_set(LED led, bool on);
