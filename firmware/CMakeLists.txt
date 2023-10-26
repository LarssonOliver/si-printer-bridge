cmake_minimum_required(VERSION 3.20)

# Pico SDK (before project)
include(vendor/pico-sdk/pico_sdk_init.cmake)

# Project
project(si-printer VERSION 1.0.0)
set(CMAKE_C_STANDARD 11)
pico_sdk_init()

# Source
add_executable(si-printer
    src/main.c)
target_link_libraries(si-printer pico_stdlib pico_unique_id tinyusb_device)
target_include_directories(si-printer PRIVATE ${CMAKE_CURRENT_LIST_DIR})
# target_compile_definitions(si-printer PRIVATE CFG_TUSB_CONFIG_FILE="chrumm/usb.h")

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
