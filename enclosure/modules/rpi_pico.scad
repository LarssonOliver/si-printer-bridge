/**
 * Copyright (C) 2024 Oliver Larsson
 *
 * This file is part of si-printer.
 *
 * si-printer is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * si-printer is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * si-printer. If not, see <https://www.gnu.org/licenses/>.
 */

// Model of a RPI Pico Rev3 board
// https://www.raspberrypi.com/products/raspberry-pi-pico/
module rpi_pico()
{
    // PCB
    PCB_THICKNESS = 1.0;
    PCB_WIDTH = 21.0;
    PCB_DEPTH = 51.0;
    PCB_Y_OFFSET = 1.3;
    PCB_HOLE_DIAMETER = 2.1;
    PCB_HOLE_DISTANCE = 11.4;
    PCB_HOLE_Y_OFFSET = 2.0;

    translate([ 0, 0, PCB_THICKNESS ])
    {
        difference()
        {
            translate([ -(PCB_WIDTH / 2), PCB_Y_OFFSET, -PCB_THICKNESS ]) cube([ PCB_WIDTH, PCB_DEPTH, PCB_THICKNESS ]);
            translate([ PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_HOLE_Y_OFFSET, -(PCB_THICKNESS + .5) ])
                cylinder(d = PCB_HOLE_DIAMETER, h = PCB_THICKNESS + 1, $fn = 32);
            translate([ -PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_HOLE_Y_OFFSET, -(PCB_THICKNESS + .5) ])
                cylinder(d = PCB_HOLE_DIAMETER, h = PCB_THICKNESS + 1, $fn = 32);
            translate([ PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_DEPTH - PCB_HOLE_Y_OFFSET, -(PCB_THICKNESS + .5) ])
                cylinder(d = PCB_HOLE_DIAMETER, h = PCB_THICKNESS + 1, $fn = 32);
            translate([ -PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_DEPTH - PCB_HOLE_Y_OFFSET, -(PCB_THICKNESS + .5) ])
                cylinder(d = PCB_HOLE_DIAMETER, h = PCB_THICKNESS + 1, $fn = 32);
        }
    }
};

module rpi_pico_standoffs(inset_diameter = 2.0, wall_thickness = 1.2, height = 5)
{
    PCB_DEPTH = 51.0;
    PCB_Y_OFFSET = 1.3;
    PCB_HOLE_DISTANCE = 11.4;
    PCB_HOLE_Y_OFFSET = 2.0;

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
    translate([ PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_DEPTH - PCB_HOLE_Y_OFFSET, 0 ]) difference()
    {
        translate([ 0, 0, -height ]) cylinder(d = inset_diameter + 2 * wall_thickness, h = height, $fn = 32);
        translate([ 0, 0, -(height + .5) ]) cylinder(d = inset_diameter, h = height + 1, $fn = 32);
    }
    translate([ -PCB_HOLE_DISTANCE / 2, PCB_Y_OFFSET + PCB_DEPTH - PCB_HOLE_Y_OFFSET, 0 ]) difference()
    {
        translate([ 0, 0, -height ]) cylinder(d = inset_diameter + 2 * wall_thickness, h = height, $fn = 32);
        translate([ 0, 0, -(height + .5) ]) cylinder(d = inset_diameter, h = height + 1, $fn = 32);
    }
}
