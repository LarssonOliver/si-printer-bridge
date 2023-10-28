#pragma once

#define CFG_TUSB_DEBUG 0
#define CFG_TUSB_OS OPT_OS_PICO

#define PIO_USB_USE_TINYUSB

#define CFG_TUH_ENABLED 1
#define CFG_TUH_RPI_PIO_USB 1

#define CFG_TUH_ENUMERATION_BUFSIZE 256

#define CFG_TUH_HUB 1
#define CFG_TUH_DEVICE_MAX (CFG_TUH_HUB ? 4 : 1)

#define CFG_TUH_CDC 1
#define CFG_TUH_CDC_CP210X 1

#define CFG_TUH_MEM_ALIGN __attribute__((aligned(4)))

#define BOARD_TUH_RHPORT CFG_TUH_RPI_PIO_USB
