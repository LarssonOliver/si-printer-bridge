#include "data_processor.h"

#include "console.h"
#include "sportident/sportident.h"

// This function is called when a new data is received from the
// serial port. The data is stored in the buffer pointed by data
// and its length is len.
void on_data_received(const uint8_t *data, uint32_t len) {

  (void)len;
  station_reply_t reply;

  if (decode_station_reply(data, &reply)) {
    console_printf("Failed to decode station reply\r\n");
    return;
  }

  if (reply.command_code == C_SI5_DET || reply.command_code == C_SI6_DET ||
      reply.command_code == C_SI9_DET) {
    si_card_t card;
    if (decode_si_det(&reply, &card))
      return;

    console_printf("Card number: %u\r\n", card.card_number);

  } else {

    console_printf("Command code: %02x\r\n", reply.command_code);
    for (unsigned int i = 0; i < reply.data_len; i++)
      console_printf("%02x", reply.data[i]);
    console_printf("\r\n");
  }
}
