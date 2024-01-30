// This module is a model of
// https://www.electrokit.com/en/product/usb-b-hona-monterad-pa-kort/
module usb_a_female_pcb()
{
    // PCB
    PCB_THICKNESS = 1.6;
    PCB_WIDTH = 17.0;
    PCB_DEPTH = 17.0;
    PCB_Y_OFFSET = 6.0;
    PCB_HOLE_DIAMETER = 3.0;
    PCB_HOLE_DISTANCE = 12.0;
    PCB_HOLE_Y_OFFSET = 11.0;

    difference()
    {
        translate([ -(PCB_WIDTH / 2), PCB_Y_OFFSET, -PCB_THICKNESS ]) cube([ PCB_WIDTH, PCB_DEPTH, PCB_THICKNESS ]);
        translate([ PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_HOLE_Y_OFFSET, -(PCB_THICKNESS + .5) ])
            cylinder(d = PCB_HOLE_DIAMETER, h = PCB_THICKNESS + 1, $fn = 32);
        translate([ -PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_HOLE_Y_OFFSET, -(PCB_THICKNESS + .5) ])
            cylinder(d = PCB_HOLE_DIAMETER, h = PCB_THICKNESS + 1, $fn = 32);
    }

    translate([ -13.1 / 2, 14 - 5, 0 ]) cube([ 13.1, 5, 7 ]);
    translate([ -13.1 / 2, 14 - 5, 0 ]) cube([ 13.1, 5, 7 ]);

    difference()
    {
        translate([ -13.1 / 2, 0, 7 - 5.75 ]) cube([ 13.1, 14, 5.75 ]);
        translate([ -12.5 / 2, -.1, 7 - 5.75 + (5.75 - 5.1) / 2 ]) cube([ 12.5, 10.1, 5.1 ]);
    }
};
