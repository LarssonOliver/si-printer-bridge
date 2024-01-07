#include <stdint.h>
#include <string.h>

#include "constants.h"
#include "sportident.h"

// Static function prototypes
static uint16_t crc(const uint8_t *command, const uint8_t len);
static int crc_check(const uint8_t *command, const uint8_t len,
                     const uint16_t expected_crc);

static int decode_cardnr(const uint8_t cardnr[4]);

// Decodes a reply form a station.
// Returns 0 on success, -1 on failure.
int si_decode_station_reply(const uint8_t *const buf,
                            si_station_reply_t *const reply) {
  if (buf == NULL || reply == NULL)
    return -1;

  const uint8_t *ptr = buf;

  if (*ptr == WAKEUP)
    ptr++;

  if (*ptr++ != STX)
    return -1;

  const uint8_t *command_start = ptr;

  const uint8_t command = *ptr++;
  const uint8_t len = *ptr++;

#if 0
  // I don't need the station id of the readout station.
  const uint8_t station[2] = {*ptr, *ptr + 1};
#endif
  ptr += 2;

  const uint8_t data_len = len - 2;
  const uint8_t *data = ptr;
  ptr += data_len;

  const uint16_t crc = *ptr << 8 | *(ptr + 1);
  ptr += 2;

  if (*ptr != ETX)
    return -1;

  if (!crc_check(command_start, sizeof(command) + sizeof(len) + len, crc))
    return -1;

  reply->command_code = command;
  reply->data_len = data_len;
  memcpy(reply->data, data, data_len);

  return 0;
}

// Decodes information of an inserted card.
// Returns 0 on success, -1 on failure.
int si_decode_det(const si_station_reply_t *const reply, si_card_t *const out) {
  if (reply == NULL || out == NULL)
    return -1;

  if (reply->data_len != 4)
    return -1;

  uint8_t data[4] = {reply->data[0], reply->data[1], reply->data[2],
                     reply->data[3]};

  if (reply->command_code == C_SI9_DET)
    data[0] = 0x00;

  out->card_number = decode_cardnr(data);

  if (reply->command_code == C_SI5_DET) {
    out->card_def = &SI5;
  } else if (reply->command_code == C_SI6_DET) {
    out->card_def = &SI6;
  } else if (reply->command_code == C_SI9_DET) {
    if (out->card_number >= 2000000 && out->card_number < 3000000)
      out->card_def = &SI8;
    else if (out->card_number >= 1000000 && out->card_number < 2000000)
      out->card_def = &SI9;
    else if (out->card_number >= 4000000 && out->card_number < 5000000)
      out->card_def = &pCard;
    else if (out->card_number >= 7000000 && out->card_number < 10000000)
      out->card_def = &SI10;
    else
      return -1;
  } else {
    return -1;
  }

  return 0;
}

// Generates a command to send to the station.
// Returns the length of the command on success, -1 on failure.
int si_build_command(uint8_t command_code, const uint8_t *params,
                     uint8_t params_len, uint8_t *out, uint8_t out_size) {
  if (out == NULL)
    return -1;

  if (params_len > 0 && params == NULL)
    return -1;

  // WAKEUP + STX + CMD + LEN + PARAMS + CRC + ETX
  const int command_length = 1 + 1 + 1 + 1 + params_len + 2 + 1;
  if (out_size < command_length)
    return -1;

  out[0] = WAKEUP;
  out[1] = STX;
  out[2] = command_code;
  out[3] = params_len;

  if (params_len > 0)
    memcpy(out + 4, params, params_len);

  // CMD + LEN + PARAMS
  const uint16_t checksum = crc(out + 2, 1 + 1 + params_len);
  out[4 + params_len] = checksum >> 8;
  out[5 + params_len] = checksum & 0xFF;
  out[6 + params_len] = ETX;

  return command_length;
}

// Generates a command to read the card.
// Returns the length of the command on success, -1 on failure.
int si_build_read_command(si_card_t *card, uint8_t *out, uint8_t out_size) {
  if (card == NULL || card->card_def == NULL || out == NULL)
    return -1;

  if (card->card_def == &SI5)
    return si_build_command(C_GET_SI5, NULL, 0, out, out_size);

  if (card->card_def == &SI6) {
    uint8_t params[] = {P_SI6_CB};
    return si_build_command(C_GET_SI6, params, sizeof(params), out, out_size);
  }

  if (card->card_def == &SI8 || card->card_def == &SI9 ||
      card->card_def == &pCard) {
    uint8_t params[] = {0};
    return si_build_command(C_GET_SI9, params, sizeof(params), out, out_size);
  }

  if (card->card_def == &SI10) {
    uint8_t params[] = {P_SI6_CB};
    return si_build_command(C_GET_SI9, params, sizeof(params), out, out_size);
  }

  return -1;
}

// Generates the next command in the sequence.
// Modifies the command in place.
// Returns the length of the command on success, -1 on failure, 0 on end of
// sequence.
int si_build_next_read_command(si_card_t *card, uint8_t *command,
                               uint8_t buffer_size) {
  if (command == NULL)
    return -1;

  const uint8_t command_code = command[2];

  if (command_code != C_GET_SI9)
    return 0;

  const uint8_t last_block = command[4];

  if (last_block == P_SI6_CB)
    // Used for SI10 and SI11 cards.
    return 0;

  if (last_block + 1 == (*card->card_def)[F_BC])
    return 0;

  const uint8_t params[] = {last_block + 1};

  return si_build_command(command_code, params, sizeof(params), command,
                          buffer_size);
}

// Compute CRC checksum for a given command
static uint16_t crc(const uint8_t *command, const uint8_t len) {

  if (len < 1)
    return 0x0000;

  int padding = len % 2;
  if (len > 2 && padding == 0)
    // I don't understand why...
    padding = 2;

  const int padded_len = len + padding;

  uint8_t buf[padded_len];

  buf[padded_len - 1] = 0x00;
  buf[padded_len - 2] = 0x00;

  memcpy(buf, command, len);

  int crc = buf[0] << 8 | buf[1];

  for (int i = 2; i < padded_len; i += 2) {
    int val = buf[i] << 8 | buf[i + 1];

    for (int j = 0; j < 16; j++) {

      if ((crc & CRC_BITF) != 0) {

        crc <<= 1;

        if ((val & CRC_BITF) != 0)
          crc += 1;

        crc ^= CRC_POLY;

      } else {

        crc <<= 1;

        if ((val & CRC_BITF) != 0)
          crc += 1;
      }

      val <<= 1;
    }
  }

  return crc & 0xFFFF;
}

// Validate the crc checksum of a command
static int crc_check(const uint8_t *command, const uint8_t len,
                     const uint16_t expected_crc) {
  return crc(command, len) == expected_crc;
}

/**
 * Decodes a 4 byte cardnr to an int. SI-Card numbering is a bit odd:
 * SI-Card 5:
 *    - byte 0:   always 0 (not stored on the card)
 *    - byte 1:   card series (stored on the card as CNS)
 *    - byte 2,3: card number
 *    - printed:  100'000*CNS + card number
 *    - nr range: 1-65'000 + 200'001-265'000 + 300'001-365'000 + 400'001-465'000
 * SI-Card 6/6* /8/9/10/11/pCard/tCard/fCard/SIAC1:
 *    - byte 0:   card series (SI6:00, SI9:01, SI8:02, pCard:04, tCard:06,
 * fCard:0E, SI10+SI11+SIAC1:0F)
 *    - byte 1-3: card number
 *    - printed:  only card number
 *    - nr range:
 *        - SI6: 500'000-999'999 + 2'003'000-2'003'400 (WM2003) +
 * 16'711'680-16'777'215 (SI6*)
 *        - SI9: 1'000'000-1'999'999, SI8: 2'000'000-2'999'999
 *        - pCard: 4'000'000-4'999'999, tCard: 6'000'000-6'999'999
 *        - SI10: 7'000'000-7'999'999, SIAC1: 8'000'000-8'999'999
 *        - SI11: 9'000'000-9'999'999, fCard: 14'000'000-14'999'999
 *
 * The card nr ranges guarantee that no ambigous values are possible
 * (500'000 = 0x07A120 > 0x04FFFF = 465'535 = highest technically possible value
 * on a SI5)
 */
static int decode_cardnr(const uint8_t cardnr[4]) {

  if (cardnr[0] != 0)
    return -1;

  int number = cardnr[1] << 16 | cardnr[2] << 8 | cardnr[3];

  if (number < 500000) {
    number &= 0xFFFF;

    if (cardnr[1] >= 2)
      number += cardnr[1] * 100000;
  }

  return number;
}

// Decode the number of a station.
// On some newer stations, two additional bits are stored in the PTD byte.
static int decode_station_number(const uint8_t raw, const uint8_t ptd) {
  if (ptd == 0x00)
    return (int)raw;

  return (((int)ptd & 0xC0) << 2) | raw;
}

// Decode a time from a raw value.
// Returns 0 on success, -1 on failure.
static int decode_time(const uint16_t raw, const uint8_t *raw_ptd,
                       si_time_t *out) {
  if (out == NULL || raw == TIME_RESET)
    return -1;

  // Documentation of the PTD byte from SportIdent
  // bit 0 - am/pm
  // bit 3...1 - day of week, 000 = Sunday, 110 = Saturday
  // bit 5...4 - week counter 0...3, relative
  // bit 7...6 - control station code number high
  // (...511)
  // week counter is not used!

  uint32_t punch_time = raw;
  uint8_t detailed = 0;
  uint8_t day_of_week = 0;

  if (raw_ptd != NULL) {
    detailed = 1;

    if (*raw_ptd & 0b1) // If PM
      punch_time += 12 * 60 * 60;

    day_of_week = (*raw_ptd & 0b1110) >> 1;
  }

  out->is_detailed = detailed;
  out->day_of_week = day_of_week;
  out->seconds_since_midnight = punch_time;

  return 0;
}

// Decodes a punch.
// Returns 0 on success, -1 on failure.
static int decode_punch(const uint8_t *punch_time_day,
                        const uint8_t *punch_time, const uint8_t *station,
                        si_punch_t *const out) {

  if (out == NULL || punch_time == NULL || station == NULL)
    return -1;

  const uint8_t time_day = punch_time_day == NULL ? 0 : *punch_time_day;
  const int station_code = decode_station_number(*station, time_day);

  si_time_t time;
  if (decode_time(punch_time[0] << 8 | punch_time[1], punch_time_day, &time))
    return -1;

  out->station = station_code;
  memcpy(&out->time, &time, sizeof(time));

  return 0;
}

// Decodes a card readout. The decoded data is stored in out.
// Returns 0 on success, -1 on failure.
int si_decode_carddata(const si_card_def_t card, const uint8_t *data,
                       si_card_readout_t *const out) {

  if (card == NULL || data == NULL || out == NULL)
    return -1;

  const uint8_t cardnr_raw[] = {0x00, data[card[F_CN2]], data[card[F_CN1]],
                                data[card[F_CN0]]};
  out->card_number = decode_cardnr(cardnr_raw);

  // Start
  const uint8_t *time_day = card[F_STD] != F_NONE ? &data[card[F_STD]] : NULL;
  const uint8_t *time = card[F_ST] != F_NONE ? &data[card[F_ST]] : NULL;
  const uint8_t *station = card[F_SN] != F_NONE ? &data[card[F_SN]] : NULL;
  decode_punch(time_day, time, station, &out->start);

  // Finish
  time_day = card[F_FTD] != F_NONE ? &data[card[F_FTD]] : NULL;
  time = card[F_FT] != F_NONE ? &data[card[F_FT]] : NULL;
  station = card[F_FN] != F_NONE ? &data[card[F_FN]] : NULL;
  decode_punch(time_day, time, station, &out->finish);

  // Check
  time_day = card[F_CTD] != F_NONE ? &data[card[F_CTD]] : NULL;
  time = card[F_CT] != F_NONE ? &data[card[F_CT]] : NULL;
  station = card[F_CHN] != F_NONE ? &data[card[F_CHN]] : NULL;
  decode_punch(time_day, time, station, &out->check);

  // Clear
  if (card[F_LT] != F_NONE) {
    // SI 5 and 9 do not store clear time.

    time_day = card[F_LTD] != F_NONE ? &data[card[F_LTD]] : NULL;
    time = card[F_LT] != F_NONE ? &data[card[F_LT]] : NULL;
    station = card[F_LN] != F_NONE ? &data[card[F_LN]] : NULL;
    decode_punch(time_day, time, station, &out->clear);
  }

  uint8_t record_count = data[card[F_RC]];
  if (card == SI5) {
    // F_RC is the index of the next record on SI5
    record_count--;
  }

  if (record_count > card[F_PM]) {
    // If there are more records than the max allowed
    record_count = card[F_PM];
  }

  out->punch_count = record_count;

  unsigned int i = card[F_P1];
  for (unsigned int p = 0; p < record_count; p++) {

    if (card == SI5 && i % 16 == 0)
      i++;

    time_day = card[F_PTD] != F_NONE ? &data[card[F_PTD] + i] : NULL;
    time = card[F_PTH] != F_NONE ? &data[card[F_PTH] + i] : NULL;
    station = card[F_CN] != F_NONE ? &data[card[F_CN] + i] : NULL;
    decode_punch(time_day, time, station, &out->punches[p]);

    i += card[F_PL];
  }

  return 0;
}
