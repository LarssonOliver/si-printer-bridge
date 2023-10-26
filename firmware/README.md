# SI-Printer Firmware

Firmware written in C built for the Raspberry Pi Pico.

## Compile

To compile a custom version, install the Pico SDK and Arm GNU Toolchain.
Set the `PICO_SDK_PATH` environment variable to the SDK directory.
Append the Arm `bin` directory to the `PATH` environment variable.
The firmware can then be prepared and compiled with:

```bash
cmake -B build
cmake --build build
```

