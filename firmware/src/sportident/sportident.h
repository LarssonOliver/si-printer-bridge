#pragma once

#include <stdint.h>

#include "constants.h"

typedef struct {
  uint8_t command_code;
  uint8_t data_len;
  uint8_t data[256 - 2];
} station_reply_t;

// Decodes a reply form a station.
// Returns 0 on success, -1 on failure.
int decode_station_reply(const uint8_t *const buf,
                         station_reply_t *const reply);

typedef struct {
  int card_number;
  const card_def_t *card;
} si_card_t;

// Decodes information of an inserted card.
// Returns 0 on success, -1 on failure.
int decode_si_det(const station_reply_t *const reply, si_card_t *const out);

// Returns the number of bytes in the command written to outbuf, -1 on error.
// int get_command(const si_reader_t *const reader, uint8_t command, uint8_t
// *argv,
//                 int argc, uint8_t outbuf[]);
