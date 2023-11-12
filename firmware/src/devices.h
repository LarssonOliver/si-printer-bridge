#pragma once

#include <stdint.h>

void devices_init(void);
void devices_tick(void);

uint32_t devices_write(uint8_t idx, const void *buf, uint32_t size);
