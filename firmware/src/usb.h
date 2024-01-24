/**
 * Copyright (C) 2024 Oliver Larsson
 *
 * This file is part of si-printer.
 *
 * si-printer is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * si-printer is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * si-printer. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#define CFG_TUSB_OS OPT_OS_PICO

#define PIO_USB_USE_TINYUSB

// DEVICE

#define CFG_TUD_ENABLED 1
#define CFG_TUD_CDC 1

#define CFG_TUD_CDC_RX_BUFSIZE 256
#define CFG_TUD_CDC_TX_BUFSIZE 256

// The USB device is not recognized if this is larger.
#define CFG_TUD_CDC_EP_BUFSIZE 64

// HOST

#define CFG_TUH_ENABLED 1
#define CFG_TUH_RPI_PIO_USB 1

#define CFG_TUH_ENUMERATION_BUFSIZE 256

#define CFG_TUH_HUB 1
#define CFG_TUH_DEVICE_MAX (CFG_TUH_HUB ? 4 : 1)

#define CFG_TUH_CDC 1
#define CFG_TUH_CDC_CP210X 1

// Only allowing the BSF8 mini reader to be recognized.
#define CFG_TUH_CDC_CP210X_PID_LIST SI_BSF8_PID

#define CFG_TUH_MEM_ALIGN __attribute__((aligned(4)))

#define CFG_TUH_API_EDPT_XFER 1

#define BOARD_TUD_RHPORT 0
#define BOARD_TUH_RHPORT CFG_TUH_RPI_PIO_USB

// Device vendor and product IDs.
#define SI_BSF8_VID 0x10C4
#define SI_BSF8_PID 0x800A
#define SI_PRINTER_VID 0x0416
#define SI_PRINTER_PID 0x5011

