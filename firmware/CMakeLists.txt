# Copyright (C) 2024 Oliver Larsson
#
# This file is part of si-printer.
#
# si-printer is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# si-printer is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# si-printer. If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.20)

# Pico SDK (before project)
include(${CMAKE_CURRENT_LIST_DIR}/vendor/pico-sdk/pico_sdk_init.cmake)

# Project
project(si-printer VERSION 1.0.0)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_TINYUSB_PATH ${CMAKE_CURRENT_LIST_DIR}/vendor/tinyusb)

pico_sdk_init()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/vendor/Pico-PIO-USB/ pico_pio_usb)

# Source
add_executable(si-printer
    ./src/main.c
    ./src/console.c
    ./src/devices.c
    ./src/printer.c
    ./src/data_processor.c
    ./src/sportident/sportident.c
    ./src/sportident/constants.c
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
)
target_link_libraries(si-printer pico_stdlib pico_multicore pico_pio_usb tinyusb_device tinyusb_host tinyusb_board)
target_link_options(si-printer PRIVATE -Xlinker --print-memory-usage)
target_include_directories(si-printer PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(si-printer PRIVATE CFG_TUSB_CONFIG_FILE="src/usb.h")

# Binary
pico_set_program_version(si-printer "${PROJECT_VERSION}")
pico_set_program_description(si-printer "SI Printer Firmware")
pico_set_binary_type(si-printer copy_to_ram)
pico_add_extra_outputs(si-printer)

# Verbose compiler warnings
if(MSVC)
    target_compile_options(si-printer PRIVATE /W4 /WX)
else()
    target_compile_options(si-printer PRIVATE -Wall -Wextra -Werror)
endif()
