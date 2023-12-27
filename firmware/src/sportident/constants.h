#pragma once

#include <stdint.h>

#define CRC_BITF 0x8000
#define CRC_POLY 0x8005

enum card_field {
  CN2 = 0, // card number byte 2
  CN1,     // card number byte 1
  CN0,     // card number byte 0
  STD,     // start time day
  SN,      // start number
  ST,      // start time
  FTD,     // finish time day
  FN,      // finish number
  FT,      // finish time
  CTD,     // check time day
  CHN,     // check number
  CT,      // check time
  LTD,     // clear time day
  LN,      // clear number
  LT,      // clear time
  RC,      // punch counter
  P1,      // first punch
  PL,      // punch data length in bytes
  PM,      // punch maximum (punches 31-36 have no time)
  CN,      // control number offset in punch record
  PTD,     // punchtime day byte offset in punch record
  PTH,     // punchtime high byte offset in punch record
  PTL,     // punchtime low byte offset in punch record
  BC,      // number of blocks on card
};

typedef uint8_t card_t[24];

const card_t SI5 = {
    6,    4,    5,    0xFF, 0xFF, 19, 0xFF, 0xFF, 21,   0xFF, 0xFF, 25,
    0xFF, 0xFF, 0xFF, 23,   32,   3,  30,   0,    0xFF, 1,    2,    0xFF,
};

const card_t SI6 = {
    11, 12, 13, 24, 25,  26, 20, 21, 22, 28, 29, 30,
    32, 33, 34, 18, 128, 4,  64, 0,  1,  2,  3,  0xFF,
};

const card_t SI8 = {
    25,   26,   27,   12, 13,  14, 16, 17, 18, 8, 9, 10,
    0xFF, 0xFF, 0xFF, 22, 136, 4,  50, 0,  1,  2, 3, 2,
};

const card_t SI9 = {
    25,   26,   27,   12, 13, 14, 16, 17, 18, 8, 9, 10,
    0xFF, 0xFF, 0xFF, 22, 56, 4,  50, 0,  1,  2, 3, 2,
};

const card_t pCard = {
    25,   26,   27,   12, 13,  14, 16, 17, 18, 8, 9, 10,
    0xFF, 0xFF, 0xFF, 22, 176, 4,  20, 0,  1,  2, 3, 2,
};

// Also SI11
const card_t SI10 = {
    25,   26,   27,   12, 13,  14, 16, 17, 18, 8, 9, 10,
    0xFF, 0xFF, 0xFF, 22, 128, 4,  64, 0,  1,  2, 3, 8,
};
