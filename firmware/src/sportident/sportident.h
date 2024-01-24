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
  const si_card_def_t *card_def;
} si_card_t;

// Decodes information of an inserted card.
// Returns 0 on success, -1 on failure.
int si_decode_det(const si_station_reply_t *const reply, si_card_t *const out);

// Generates a command to send to the station.
// Returns the length of the command on success, -1 on failure.
int si_build_command(uint8_t command_code, const uint8_t *params,
                     uint8_t params_len, uint8_t *out, uint8_t out_size);

// Generates a command to read the card.
// Returns the length of the command on success, -1 on failure.
int si_build_read_command(si_card_t *card, uint8_t *out, uint8_t out_size);

// Generates the next command in the sequence.
// Modifies the command in place.
// Returns the length of the command on success, -1 on failure, 0 on end of
// sequence.
int si_build_next_read_command(si_card_t *card, uint8_t *out, uint8_t out_size);

// This struct could be reduced in size.
typedef struct {
  uint8_t is_detailed : 1;
  uint8_t day_of_week : 7;
  uint32_t seconds_since_midnight : (8 * 3); // Only 17 bits are required.
} si_time_t;

typedef struct {
  uint8_t is_punched : 1;
  uint32_t station : sizeof(uint32_t) * 8 - 1;
  si_time_t time;
} si_punch_t;

typedef struct {
  int32_t card_number;
  si_punch_t start;
  si_punch_t finish;
  si_punch_t check;
  si_punch_t clear;
  unsigned int punch_count;
  si_punch_t punches[MAX_PUNCHES];
} si_card_readout_t;

// Decodes a card readout. The decoded data is stored in out.
// Returns 0 on success, -1 on failure.
int si_decode_carddata(const si_card_def_t card, const uint8_t *data,
                       si_card_readout_t *const out);
