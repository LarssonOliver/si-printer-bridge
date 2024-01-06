#include <string.h>

#include "data_processor.h"

#include "console.h"
#include "devices.h"
#include "usb.h"

#include "sportident/sportident.h"

static struct {
  si_card_t card;
} s_state;

static uint8_t s_receive_buffer[CFG_TUD_CDC_RX_BUFSIZE];
static int s_receive_data_size = 0;

static void handle_data(const uint8_t *data, uint32_t len);
static void test_time(void);

// This function is called when a new data is received from the
// serial port. The data is stored in the buffer pointed by data
// and its length is len.
void on_data_received(const uint8_t *data, uint32_t len) {
  if (data == (void *)0 || len == 0)
    return;

  if (s_receive_data_size + len > sizeof(s_receive_buffer)) {
    console_printf("Receive buffer overflow.\r\n");
    return;
  }

  if (data[0] == STX || (data[0] == WAKEUP && data[1] == STX))
    s_receive_data_size = 0;

  memcpy(s_receive_buffer + s_receive_data_size, data, len);
  s_receive_data_size += len;

  if (s_receive_buffer[s_receive_data_size - 1] == ETX) {
    handle_data(s_receive_buffer, s_receive_data_size);
    s_receive_data_size = 0;
  }
}

static void handle_data(const uint8_t *data, uint32_t len) {
  si_station_reply_t reply;

  if (si_decode_station_reply(data, &reply)) {
    console_printf("Failed to decode station reply with data: ");
    for (unsigned int i = 0; i < len; i++)
      console_printf("%02x", data[i]);
    console_printf("\r\n");
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

__attribute__((unused))
static void test_time(void) {
  uint8_t buf[32];

  // const uint8_t params[] = {0x00, 0x80};
  // const int cmdlen =
  //     si_build_command(0x83, params, sizeof(params), buf, sizeof(buf));

  const int cmdlen =
      si_build_command(0x77, (void *)0, 0, buf, sizeof(buf));

  if (cmdlen < 0)
    return;

  devices_write(0, buf, cmdlen);
}
