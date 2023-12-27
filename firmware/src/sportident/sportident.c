#include <string.h>

#include "constants.h"
#include "sportident.h"

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
__attribute__((unused)) static int check_crc(const uint8_t len,
                                             const uint8_t command[],
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

    if (cardnr[1] < 2)
      return number;

    return cardnr[1] * 100000 + number;
  }

  return number;
}

// Decode data read from a card
__attribute__((unused)) static void decode_carddata(const uint8_t *const data,
                                                    const card_t card) {

  const uint8_t cardnr_raw[] = {0x00, data[card[CN2]], data[card[CN1]],
                                data[card[CN0]]};

  __attribute__((unused)) const int cardnr = decode_cardnr(cardnr_raw);
}
