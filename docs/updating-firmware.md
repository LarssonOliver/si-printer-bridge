# Updating the Microcontroller Firmware

These instructions describe the process of loading a new firmware (in `.uf2` 
format) onto the Raspberry PI Pico microcontroller (RP2040).

Find the BOOTSEL button on the Pico microcontroller.

![Illustration of Pico and BOOTSEL button](https://projects-static.raspberrypi.org/projects/getting-started-with-the-pico/725a421f3b51a5674c539d6953db5f1892509475/en/images/Pico-bootsel.png)

**NOTE:** Do not power the Pico using any external power source (such as a 
USB powerbank during this procedure.

Depress the BOOTSEL button while plugging the microcontroller into a computer
using the micro-USB port on the short end of the Pico.

This makes the Pico show up as a drive in the operating system. Drag and 
drop the `.uf2` firmware file onto the drive.

Unplug the microcontroller.

The firmware is now updated and the device may be started as normal.

