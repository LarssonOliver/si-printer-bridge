#include <string.h>

#include "constants.h"
#include "sportident.h"

// Compute CRC checksum for a given command
static uint16_t crc(uint8_t len, uint8_t command[]) {

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
