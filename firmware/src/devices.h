#pragma once

#include <stdint.h>

// Callback that is called when input is received from a device.
typedef void (*input_callback_t)(const uint8_t *buf, uint32_t size);

void devices_init(void);
void devices_tick(void);

// Register a callback that will be called when input is received from a device.
void devices_register_input_callback(input_callback_t cb);

// Write to a device. Returns the number of bytes written.
uint32_t devices_reader_write(const void *buf, uint32_t size);
