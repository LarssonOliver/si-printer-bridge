#pragma once

#include <stdint.h>

typedef struct {
} si_reader_t;

// Returns the number of bytes in the command written to outbuf, -1 on error.
int get_command(const si_reader_t *const reader, uint8_t command, uint8_t *argv,
                int argc, uint8_t outbuf[]);

