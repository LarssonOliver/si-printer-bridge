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

#pragma once

#include <stdint.h>

// Callback that is called when input is received from a device.
typedef void (*input_callback_t)(const uint8_t *buf, uint32_t size);

// Initialize USB host driver.
void devices_init(void);
// Initialize the 2nd USB port, should be called about 1s after devices_init.
void devices_init2(void);

void devices_tick(void);

// Register a callback that will be called when input is received from a device.
void devices_register_input_callback(input_callback_t cb);

// Write to a device. Returns the number of bytes written.
uint32_t devices_reader_write(const void *buf, uint32_t size);
