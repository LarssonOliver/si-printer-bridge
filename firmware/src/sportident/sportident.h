#pragma once

#include <stdint.h>

#include "constants.h"

typedef struct {
  uint8_t command_code;
  uint8_t data_len;
  uint8_t data[256 - 2];
} si_station_reply_t;

// Decodes a reply form a station.
// Returns 0 on success, -1 on failure.
int si_decode_station_reply(const uint8_t *const buf,
                            si_station_reply_t *const reply);

typedef struct {
  int card_number;
  const card_def_t *card_def;
} si_card_t;

// Decodes information of an inserted card.
// Returns 0 on success, -1 on failure.
int si_decode_det(const si_station_reply_t *const reply, si_card_t *const out);

// Generates a command to send to the station.
// Returns the length of the command on success, -1 on failure.
int si_build_command(uint8_t command_code, const uint8_t *params,
                     uint8_t params_len, uint8_t *out, uint8_t out_size);
