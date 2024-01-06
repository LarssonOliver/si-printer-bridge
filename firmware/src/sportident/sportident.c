#include <stdint.h>
#include <string.h>

#include "constants.h"
#include "sportident.h"

// Static function prototypes
static uint16_t crc(const uint8_t len, const uint8_t command[]);
static int crc_check(const uint8_t len, const uint8_t command[],
                     const uint16_t expected_crc);

static int decode_cardnr(const uint8_t cardnr[4]);

// Decodes a reply form a station.
// Returns 0 on success, -1 on failure.
int decode_station_reply(const uint8_t *const buf,
                         station_reply_t *const reply) {
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

  if (!crc_check(sizeof(command) + sizeof(len) + len, command_start, crc))
    return -1;

  reply->command_code = command;
  reply->data_len = data_len;
  memcpy(reply->data, data, data_len);

  return 0;
}

// Decodes information of an inserted card.
// Returns 0 on success, -1 on failure.
int decode_si_det(const station_reply_t *const reply, si_card_t *const out) {
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
    out->card = &SI5;
  } else if (reply->command_code == C_SI6_DET) {
    out->card = &SI6;
  } else if (reply->command_code == C_SI9_DET) {
    if (out->card_number >= 2000000 && out->card_number < 3000000)
      out->card = &SI8;
    else if (out->card_number >= 1000000 && out->card_number < 2000000)
      out->card = &SI9;
    else if (out->card_number >= 4000000 && out->card_number < 5000000)
      out->card = &pCard;
    else if (out->card_number >= 7000000 && out->card_number < 10000000)
      out->card = &SI10;
    else
      return -1;
  } else {
    return -1;
  }

  return 0;
}

// Compute CRC checksum for a given command
static uint16_t crc(const uint8_t len, const uint8_t command[]) {

  if (len < 1)
    return 0x0000;

  const int padding = len % 2 == 0 ? 2 : 1;
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
static int crc_check(const uint8_t len, const uint8_t command[],
                     const uint16_t expected_crc) {
  return crc(len, command) == expected_crc;
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

// Decode data read from a card
__attribute__((unused)) static void decode_carddata(const uint8_t *const data,
                                                    const card_def_t card) {

  const uint8_t cardnr_raw[] = {0x00, data[card[F_CN2]], data[card[F_CN1]],
                                data[card[F_CN0]]};
  __attribute__((unused)) const int cardnr = decode_cardnr(cardnr_raw);

  uint8_t time_raw = card[F_STD] != F_NONE ? data[card[F_STD]] : 0x00;
  // const time start_time
  uint8_t station_raw = card[F_SN] != F_NONE ? data[card[F_SN]] : 0x00;
  __attribute__((unused)) const int start_station =
      decode_station_number(station_raw, time_raw);

  time_raw = card[F_FTD] != F_NONE ? data[card[F_FTD]] : 0x00;
  // const time start_time
  station_raw = card[F_FN] != F_NONE ? data[card[F_FN]] : 0x00;
  __attribute__((unused)) const int finish_station =
      decode_station_number(station_raw, time_raw);

  time_raw = card[F_CTD] != F_NONE ? data[card[F_CTD]] : 0x00;
  // const time start_time
  station_raw = card[F_CHN] != F_NONE ? data[card[F_CHN]] : 0x00;
  __attribute__((unused)) const int check_station =
      decode_station_number(station_raw, time_raw);

  if (card[F_LT] != F_NONE) {
    // SI 5 and 9 cannot store clear time.

    time_raw = card[F_LTD] != F_NONE ? data[card[F_LTD]] : 0x00;
    // const time start_time
    station_raw = card[F_LN] != F_NONE ? data[card[F_LN]] : 0x00;
    __attribute__((unused)) const int check_station =
        decode_station_number(station_raw, time_raw);
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

  unsigned int i = card[F_P1];
  for (unsigned int p = 0; p < record_count; p++) {

    if (card == SI5 && i % 16 == 0)
      i++;

    // TODO

    i += card[F_PL];
  }
}
