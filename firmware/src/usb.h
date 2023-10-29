#pragma once

#define CFG_TUSB_OS OPT_OS_PICO

#define PIO_USB_USE_TINYUSB

#define CFG_TUD_ENABLED 1

// DEVICE

#define CFG_TUD_ENDPOINT0_SIZE 64
#define CFG_TUD_CDC 1

#define CFG_TUD_CDC_RX_BUFSIZE 256
#define CFG_TUD_CDC_TX_BUFSIZE 256
#define CFG_TUD_CDC_EP_BUFSIZE 64

// HOST

#define CFG_TUH_ENABLED 1
#define CFG_TUH_RPI_PIO_USB 1

#define CFG_TUH_ENUMERATION_BUFSIZE 256

#define CFG_TUH_HUB 1
#define CFG_TUH_DEVICE_MAX (CFG_TUH_HUB ? 4 : 1)

#define CFG_TUH_CDC 1
#define CFG_TUH_CDC_FTDI 1
#define CFG_TUH_CDC_CP210X 1

#define CFG_TUH_MEM_ALIGN __attribute__((aligned(4)))

#define CP210X_PID_LIST 0xEA60, 0xEA70, 0x800A

// Set Line Control state on enumeration/mounted:
// DTR ( bit 0), RTS (bit 1)
/* #define CFG_TUH_CDC_LINE_CONTROL_ON_ENUM 0x03 */

// Set Line Coding on enumeration/mounted, value for cdc_line_coding_t
// bit rate = 115200, 1 stop bit, no parity, 8 bit data width
/* #define CFG_TUH_CDC_LINE_CODING_ON_ENUM                                        \ */
/*   { 38400, CDC_LINE_CONDING_STOP_BITS_1, CDC_LINE_CODING_PARITY_NONE, 8 } */

#define BOARD_TUD_RHPORT 0
#define BOARD_TUH_RHPORT CFG_TUH_RPI_PIO_USB
