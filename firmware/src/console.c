#include <pico/unique_id.h>
#include <stdarg.h>
#include <stdio.h>
#include <tusb.h>

#include "console.h"

void console_init(void) { tud_init(BOARD_TUD_RHPORT); }

void console_tick(void) {
  if (!tud_inited())
    return;

  tud_task();
}

#define CONSOLE_BUF_SIZE 256

int console_printf(const char *format, ...) {
  if (!tud_inited())
    return -1;

  va_list argptr;
  va_start(argptr, format);

  char buf[CONSOLE_BUF_SIZE];
  vsnprintf(buf, CONSOLE_BUF_SIZE, format, argptr);
  va_end(argptr);

  return tud_cdc_write_str(buf);
  sleep_ms(1);
}

// Device Descriptor

// Default from TinyUSB
#define USB_VID 0xCAFE
#define USB_PID                                                                \
  (0x4000 | CFG_TUD_CDC << 0 | CFG_TUD_MSC << 1 | CFG_TUD_HID << 2 |           \
   CFG_TUD_MIDI << 3 | CFG_TUD_VENDOR << 4)

const tusb_desc_device_t device_descriptor = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01};

uint8_t const *tud_descriptor_device_cb(void) {
  return (uint8_t const *)&device_descriptor;
}

// Config Descriptor

enum { ITF_NUM_CDC = 0, ITF_NUM_CDC_DATA, ITF_NUM_TOTAL };

#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT 0x02
#define EPNUM_CDC_IN 0x82

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)

uint8_t const desc_fs_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT,
                       EPNUM_CDC_IN, CFG_TUD_CDC_EP_BUFSIZE),
};

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  (void)index;
  return desc_fs_configuration;
}

// String Descriptors

#pragma pack(push, 1)

const struct {
  uint8_t length;
  uint8_t type;
  uint16_t string[1];
} language = {sizeof(language), TUSB_DESC_STRING, {0x0409}}; // English

const struct {
  uint8_t length;
  uint8_t type;
  uint16_t string[17];
} manufaturer = {sizeof(manufaturer),
                 TUSB_DESC_STRING,
                 {'l', 'a', 'r', 's', 's', 'o', 'n', 'o', 'l', 'i', 'v', 'e',
                  'r', '.', 'c', 'o', 'm'}};

const struct {
  uint8_t length;
  uint8_t type;
  uint16_t string[10];
} product = {sizeof(product),
             TUSB_DESC_STRING,
             {'S', 'I', '-', 'P', 'r', 'i', 'n', 't', 'e', 'r'}};

#define SERIAL_CHAR_COUNT (PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2)

uint16_t serial[SERIAL_CHAR_COUNT + 1] = {
    TUSB_DESC_STRING << 8 | sizeof(serial)}; // Value read and set in callback

#pragma pack(pop)

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t lang) {
  (void)lang;

  char board_id_string_utf8[SERIAL_CHAR_COUNT + 1];

  switch (index) {
  case 0:
    return (const uint16_t *)&language;
  case 1:
    return (const uint16_t *)&manufaturer;
  case 2:
    return (const uint16_t *)&product;
  case 3:
    pico_get_unique_board_id_string(board_id_string_utf8,
                                    sizeof(board_id_string_utf8));

    // UTF8 -> UTF-16 Conversion
    for (unsigned int i = 0; i < SERIAL_CHAR_COUNT; i++) {
      serial[1 + i] = board_id_string_utf8[i];
    }

    return (const uint16_t *)&serial;
  default:
    return NULL;
  }
}
