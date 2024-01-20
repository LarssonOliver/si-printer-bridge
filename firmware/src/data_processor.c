#include <stdint.h>
#include <string.h>

#include "data_processor.h"

#include "console.h"
#include "devices.h"
#include "printer.h"
#include "usb.h"

#include "sportident/sportident.h"

// Static variables
static si_card_t s_card;
static si_card_readout_t s_card_readout;

static uint8_t s_receive_buffer[CFG_TUD_CDC_RX_BUFSIZE];
static int s_receive_data_size = 0;

static uint8_t s_read_data_buffer[1024];
static int s_read_data_size = 0;

static uint8_t s_next_read_command[16];
static int s_next_read_command_len = 0;

// Function prototypes
static void handle_data(const uint8_t *data, uint32_t len);
static void read_card(void);
static void collect_read_data(const uint8_t *data, uint32_t len);

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
    if (si_decode_det(&reply, &s_card))
      goto card_removed;

    console_printf("Card number: %u\r\n", s_card.card_number);
    read_card();
    break;

  case C_SI_REM:
    console_printf("Card removed\r\n");
  card_removed:
    s_card.card_number = 0;
    s_card.card_def = (void *)0;
    s_next_read_command_len = 0;
    break;

  case C_GET_SI5:
  case C_GET_SI6:
  case C_GET_SI9:
    collect_read_data(reply.data, reply.data_len);
    break;

  default:
    console_printf("Command code unknown: %02x\r\nCommand data: ",
                   reply.command_code);
    for (unsigned int i = 0; i < reply.data_len; i++)
      console_printf("%02x", reply.data[i]);
    console_printf("\r\n");
    break;
  }
}

static void send_next_read_command(void) {
  if (s_next_read_command_len == 0)
    return;

  devices_reader_write(s_next_read_command, s_next_read_command_len);

  s_next_read_command_len = si_build_next_read_command(
      &s_card, s_next_read_command, sizeof(s_next_read_command));
}

static void read_card(void) {
  if (s_card.card_number == 0 || s_card.card_def == (void *)0)
    return;

  s_next_read_command_len = si_build_read_command(&s_card, s_next_read_command,
                                                  sizeof(s_next_read_command));

  s_read_data_size = 0;
  send_next_read_command();
}

static void collect_read_data(const uint8_t *data, uint32_t len) {
  static uint8_t frames_read = 0;

  if (s_read_data_size + len > sizeof(s_read_data_buffer))
    return;

  if (s_read_data_size == 0)
    frames_read = 0;

  char offset = s_card.card_def == &SI5 ? 0 : 1;

  memcpy(s_read_data_buffer + s_read_data_size, data + offset, len - offset);
  s_read_data_size += len - offset;

  frames_read++;

  if (frames_read < (*s_card.card_def)[F_RF]) {
    // We don't have all the data yet.
    if (s_next_read_command_len > 0)
      send_next_read_command();

    return;
  }

  console_printf("All data gathered, length: %u\r\n", s_read_data_size);

  if (si_decode_carddata(*s_card.card_def, s_read_data_buffer,
                         &s_card_readout)) {
    console_printf("Failed to decode card data.\r\n");
    return;
  }

  console_printf("Card readout:\r\n");
  console_printf("  Card number: %u\r\n", s_card_readout.card_number);

  console_printf("  Start station: %u\r\n", s_card_readout.start.station);
  console_printf("  Finish station: %u\r\n", s_card_readout.finish.station);
  console_printf("  Check station: %u\r\n", s_card_readout.check.station);
  console_printf("  Clear station: %u\r\n", s_card_readout.clear.station);

  console_printf("  Punch count: %u\r\n", s_card_readout.punch_count);

  for (unsigned int i = 0; i < s_card_readout.punch_count; i++) {
    console_printf("  Punch %u: (%u)\r\n", i + 1,
                   s_card_readout.punches[i].station);
  }

  // Send for printing.
  printer_print(&s_card_readout);
}
