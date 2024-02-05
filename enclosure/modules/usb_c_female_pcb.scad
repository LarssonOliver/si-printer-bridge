/**
 * Copyright (C) 2024 Oliver Larsson
 *
 * This file is part of si-printer-bridge.
 *
 * si-printer-bridge is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * si-printer-bridge is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * si-printer-bridge. If not, see <https://www.gnu.org/licenses/>.
 */

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

    translate([ 0, 0, PCB_THICKNESS ])
    {
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
                rotate(90, [ -1, 0, 0 ]) translate([ -(8.9 - 3.3) / 2, -3.3 / 2, 0 ])
                    cylinder(d = 3.3, h = 7.3, $fn = 32);
                rotate(90, [ -1, 0, 0 ]) translate([ (8.9 - 3.3) / 2, -3.3 / 2, 0 ])
                    cylinder(d = 3.3, h = 7.3, $fn = 32);
            }
            translate([ -(8.9 - 3.3) / 2, -.5, (3.3 - 2.5) / 2 ]) cube([ 8.9 - 3.3, 7.3, 2.5 ]);
            rotate(90, [ -1, 0, 0 ]) translate([ -(8.9 - 3.3) / 2, -3.3 / 2, -.5 ])
                cylinder(d = 2.5, h = 7.3, $fn = 32);
            rotate(90, [ -1, 0, 0 ]) translate([ (8.9 - 3.3) / 2, -3.3 / 2, -.5 ]) cylinder(d = 2.5, h = 7.3, $fn = 32);
        }
    }
};

module usb_c_standoffs(inset_diameter = 2.0, wall_thickness = 1.2, height = 5)
{
    PCB_Y_OFFSET = 1.0;
    PCB_HOLE_DISTANCE = 15.2;
    PCB_HOLE_Y_OFFSET = 3.5;

    translate([ PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_HOLE_Y_OFFSET, 0 ]) difference()
    {
        translate([ 0, 0, -height ]) cylinder(d = inset_diameter + 2 * wall_thickness, h = height, $fn = 32);
        translate([ 0, 0, -(height + .5) ]) cylinder(d = inset_diameter, h = height + 1, $fn = 32);
    }
    translate([ -PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_HOLE_Y_OFFSET, 0 ]) difference()
    {
        translate([ 0, 0, -height ]) cylinder(d = inset_diameter + 2 * wall_thickness, h = height, $fn = 32);
        translate([ 0, 0, -(height + .5) ]) cylinder(d = inset_diameter, h = height + 1, $fn = 32);
    }
}

module usb_c_cutout(thickness = 10)
{
    translate([ -10 / 2, -thickness, .70 ]) cube([ 10, thickness + 4, 4.0 ]);
};
