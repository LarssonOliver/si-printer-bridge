#include "data_processor.h"

#include "console.h"
#include "devices.h"

#include "sportident/sportident.h"
#include "sportident/constants.h"

static struct {
  si_card_t card;
} s_state;

static void test_time(void);

// This function is called when a new data is received from the
// serial port. The data is stored in the buffer pointed by data
// and its length is len.
void on_data_received(const uint8_t *data, uint32_t len) {

  (void)len;
  si_station_reply_t reply;

  if (si_decode_station_reply(data, &reply)) {
    console_printf("Failed to decode station reply\r\n");
    return;
  }

  switch (reply.command_code) {
  case C_SI5_DET:
  case C_SI6_DET:
  case C_SI9_DET:
    if (si_decode_det(&reply, &s_state.card))
      goto card_removed;

    console_printf("Card number: %u\r\n", s_state.card.card_number);
    break;

  case C_SI_REM:
    console_printf("Card removed\r\n");
    test_time();
  card_removed:
    s_state.card.card_number = 0;
    s_state.card.card_def = (void *)0;
    break;

  default:
    console_printf("Command code unknown: %02x\r\n", reply.command_code);
    for (unsigned int i = 0; i < reply.data_len; i++)
      console_printf("%02x", reply.data[i]);
    console_printf("\r\n");
    break;
  }
}

static void test_time(void) {
  uint8_t buf[32];

  const int cmdlen = si_build_command(C_GET_TIME, (void *)0, 0, buf, sizeof(buf));
  if (cmdlen < 0)
    return;

  devices_write(0, buf, cmdlen);
}

