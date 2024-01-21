#pragma once

#include <stdint.h>

#include "sportident/sportident.h"

// Initialize a printer. Returns 0 on success.
int printer_init(uint8_t dev_addr);

// Print a card readout. Returns 0 on success.
int printer_print(const si_card_readout_t *readout);

// Register a callback to be called when the printer is done printing.
void printer_register_done_cb(void (*cb)(void));
