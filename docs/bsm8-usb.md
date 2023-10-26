# BSM8-USB Reader

This document details the reverse engineered serial protocol.

Some details may be found in the [datasheet](https://docs.sportident.com/datasheets/en/datasheets/bsm8_datasheet_en.html).

## Serial Connection

**38400 baud**

## Functions

Here are some observed behavior.

### Beep

**Prerequisites:**
- Stick inserted.

**Call:** `0xFF06`

**Return:** -

**Description:**
The base station beeps repeatedly until the stick is removed, at which
point the beeping stops.

