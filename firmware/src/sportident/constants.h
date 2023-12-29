#pragma once

#include <stdint.h>

#define CRC_BITF 0x8000
#define CRC_POLY 0x8005

enum card_field {
  F_CN2 = 0, // card number byte 2
  F_CN1,     // card number byte 1
  F_CN0,     // card number byte 0
  F_STD,     // start time day
  F_SN,      // start number
  F_ST,      // start time
  F_FTD,     // finish time day
  F_FN,      // finish number
  F_FT,      // finish time
  F_CTD,     // check time day
  F_CHN,     // check number
  F_CT,      // check time
  F_LTD,     // clear time day
  F_LN,      // clear number
  F_LT,      // clear time
  F_RC,      // punch counter
  F_P1,      // first punch
  F_PL,      // punch data length in bytes
  F_PM,      // punch maximum (punches 31-36 have no time)
  F_CN,      // control number offset in punch record
  F_PTD,     // punchtime day byte offset in punch record
  F_PTH,     // punchtime high byte offset in punch record
  F_PTL,     // punchtime low byte offset in punch record
  F_BC,      // number of blocks on card
};

typedef uint8_t card_t[24];

#define F_NONE 0xFF

const card_t SI5 = {
    6,  4,      5,      F_NONE, F_NONE, 19,     F_NONE, F_NONE,
    21, F_NONE, F_NONE, 25,     F_NONE, F_NONE, F_NONE, 23,
    32, 3,      30,     0,      F_NONE, 1,      2,      F_NONE,
};

const card_t SI6 = {
    11, 12, 13, 24, 25,  26, 20, 21, 22, 28, 29, 30,
    32, 33, 34, 18, 128, 4,  64, 0,  1,  2,  3,  F_NONE,
};

const card_t SI8 = {
    25,     26,     27,     12, 13,  14, 16, 17, 18, 8, 9, 10,
    F_NONE, F_NONE, F_NONE, 22, 136, 4,  50, 0,  1,  2, 3, 2,
};

const card_t SI9 = {
    25,     26,     27,     12, 13, 14, 16, 17, 18, 8, 9, 10,
    F_NONE, F_NONE, F_NONE, 22, 56, 4,  50, 0,  1,  2, 3, 2,
};

const card_t pCard = {
    25,     26,     27,     12, 13,  14, 16, 17, 18, 8, 9, 10,
    F_NONE, F_NONE, F_NONE, 22, 176, 4,  20, 0,  1,  2, 3, 2,
};

// Also SI11
const card_t SI10 = {
    25,     26,     27,     12, 13,  14, 16, 17, 18, 8, 9, 10,
    F_NONE, F_NONE, F_NONE, 22, 128, 4,  64, 0,  1,  2, 3, 8,
};
