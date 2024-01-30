include <config.scad>

use <modules/usb_a_female_pcb.scad>
use <modules/usb_c_female_pcb.scad>

translate([ 30, 0, 0 ]) usb_a_female_pcb();
usb_c_female_pcb();
