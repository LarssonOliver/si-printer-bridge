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

        translate([ -13.1 / 2, 14 - 5, 0 ]) cube([ 13.1, 5, 7 ]);
        translate([ -13.1 / 2, 14 - 5, 0 ]) cube([ 13.1, 5, 7 ]);

        difference()
        {
            translate([ -13.1 / 2, 0, 7 - 5.75 ]) cube([ 13.1, 14, 5.75 ]);
            translate([ -12.5 / 2, -.1, 7 - 5.75 + (5.75 - 5.1) / 2 ]) cube([ 12.5, 10.1, 5.1 ]);
        }
    }
};

module usb_a_standoffs(inset_diameter = 4.0, wall_thickness = 1.6, height = 5)
{
    PCB_Y_OFFSET = 6.0;
    PCB_HOLE_DISTANCE = 12.0;
    PCB_HOLE_Y_OFFSET = 11.0;

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
};

module usb_a_cutout(thickness = 10)
{
    translate([ -15 / 2, -thickness, 1.6 + .5 ]) cube([ 15, thickness + 4, 7.5 ]);
};
