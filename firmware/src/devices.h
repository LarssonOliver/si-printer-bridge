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
