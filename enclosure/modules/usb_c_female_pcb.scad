// This module is a model of
// https://www.pololu.com/product/3411
module usb_c_female_pcb()
{
    // PCB
    PCB_THICKNESS = 1.0;
    PCB_WIDTH = 20.3;
    PCB_DEPTH = 12.7;
    PCB_Y_OFFSET = 1.0;
    PCB_HOLE_DIAMETER = 2.0;
    PCB_HOLE_DISTANCE = 15.2;
    PCB_HOLE_Y_OFFSET = 3.5;

    difference()
    {
        translate([ -(PCB_WIDTH / 2), PCB_Y_OFFSET, -PCB_THICKNESS ]) cube([ PCB_WIDTH, PCB_DEPTH, PCB_THICKNESS ]);
        translate([ PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_HOLE_Y_OFFSET, -(PCB_THICKNESS + .5) ])
            cylinder(d = PCB_HOLE_DIAMETER, h = PCB_THICKNESS + 1, $fn = 32);
        translate([ -PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_HOLE_Y_OFFSET, -(PCB_THICKNESS + .5) ])
            cylinder(d = PCB_HOLE_DIAMETER, h = PCB_THICKNESS + 1, $fn = 32);
    }

    difference()
    {
        union()
        {
            translate([ -(8.9 - 3.3) / 2, 0, 0 ]) cube([ 8.9 - 3.3, 7.3, 3.3 ]);
            rotate(90, [ -1, 0, 0 ]) translate([ -(8.9 - 3.3) / 2, -3.3 / 2, 0 ]) cylinder(d = 3.3, h = 7.3, $fn = 32);
            rotate(90, [ -1, 0, 0 ]) translate([ (8.9 - 3.3) / 2, -3.3 / 2, 0 ]) cylinder(d = 3.3, h = 7.3, $fn = 32);
        }
        translate([ -(8.9 - 3.3) / 2, -.5, (3.3 - 2.5) / 2 ]) cube([ 8.9 - 3.3, 7.3, 2.5 ]);
        rotate(90, [ -1, 0, 0 ]) translate([ -(8.9 - 3.3) / 2, -3.3 / 2, -.5 ]) cylinder(d = 2.5, h = 7.3, $fn = 32);
        rotate(90, [ -1, 0, 0 ]) translate([ (8.9 - 3.3) / 2, -3.3 / 2, -.5 ]) cylinder(d = 2.5, h = 7.3, $fn = 32);
    }
};
