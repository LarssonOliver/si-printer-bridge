# SI-Printer Firmware

Firmware written in C built for the Raspberry Pi Pico.

Much of the reverse engineering is based on
[sportident-python](https://github.com/per-magnusson/sportident-python), which
is licensed under the GPL-3.0 license. You should have received a copy of the
GNU General Public License along with this program. If not, see
<[http://www.gnu.org/licenses/](http://www.gnu.org/licenses/)>.

All reimplementation of sportident-python may be found in the
[sportident](./src/sportident/) directory.

## Compile

To compile a custom version, install the Pico SDK and Arm GNU Toolchain.
Set the `PICO_SDK_PATH` environment variable to the SDK directory.
Append the Arm `bin` directory to the `PATH` environment variable.
The firmware can then be prepared and compiled with:

```bash
cmake -B build
cmake --build build
```

## Install

Hold down the `BOOTSEL` button on the Pico while plugging in the USB cable.
It should show up as a USB storage device. Drag-and-drop the `uf2` binary from
the build directory onto the device. The Pico should automatically restart and
run the firmware.

Alternatively, install `picotool` and run:

```bash
picotool load build/si-printer.uf2 --force
picotool reboot
```

