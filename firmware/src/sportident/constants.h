#pragma once

#include <stdint.h>

// Common control codes
#define STX 0x02    // Start of transmission
#define ETX 0x03    // End of transmission
#define ACK 0x06    // Acknowledge (beep until card removed)
#define NAK 0x15    // Not acknowledge
#define DLE 0x10    // Data link escape
#define WAKEUP 0xFF // This byte is sent first to the station

// Protocol commands
#define C_SI5_DET 0xE5  // Detect SI5 (inserted)
#define C_SI6_DET 0xE6  // Detect SI6 (inserted)
#define C_SI9_DET 0xE8  // Detect SI6 (inserted)
#define C_SI_REM 0xE7   // SI card removed
#define C_GET_TIME 0xF7 // Get station time
#define C_GET_SI5 0xB1  // Get SI5 card data
#define C_GET_SI6 0xE1  // Get SI6 card data
#define C_GET_SI9 0xEF  // Get SI9 card data

// Protocol parameters
#define P_SI6_CB 0x08 // SI6 card blocks

#define CRC_BITF 0x8000
#define CRC_POLY 0x8005

#define MAX_PUNCHES 64 // SI6 and SI10

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
  F_RF,      // number of read frames for card
};

// Field does not exist on this card type
#define F_NONE 0xFF

typedef uint8_t si_card_def_t[25];

extern const si_card_def_t SI5;
extern const si_card_def_t SI6;
extern const si_card_def_t SI8;
extern const si_card_def_t SI9;
extern const si_card_def_t pCard;
extern const si_card_def_t SI10; // Also SI11
