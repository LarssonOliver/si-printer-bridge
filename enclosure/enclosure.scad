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

include <config.scad>

use <modules/rpi_pico.scad>
use <modules/usb_a_female_pcb.scad>
use <modules/usb_c_female_pcb.scad>

module box()
{
    difference()
    {
        translate([ -WALL_THICKNESS, -WALL_THICKNESS, -WALL_THICKNESS ]) union()
        {
            translate([ 0, WALL_THICKNESS + CORNER_INSET_SIZE / 2, 0 ]) cube([
                INTERNAL_WIDTH + 2 * WALL_THICKNESS, INTERNAL_DEPTH - CORNER_INSET_SIZE, INTERNAL_HEIGHT +
                WALL_THICKNESS
            ]);
            translate([ WALL_THICKNESS + CORNER_INSET_SIZE / 2, 0, 0 ]) cube([
                INTERNAL_WIDTH - CORNER_INSET_SIZE, INTERNAL_DEPTH + 2 * WALL_THICKNESS, INTERNAL_HEIGHT +
                WALL_THICKNESS
            ]);
        };
        cube([ INTERNAL_WIDTH, INTERNAL_DEPTH, INTERNAL_HEIGHT + .1 ]);

        // Component cutouts
        translate([ INTERNAL_WIDTH - 20, -2, STANDOFF_HEIGHT ]) usb_a_cutout();
        translate([ INTERNAL_WIDTH - 45, -2, STANDOFF_HEIGHT ]) usb_a_cutout();
        translate([ -.5, 20, STANDOFF_HEIGHT ]) rotate(90, [ 0, 0, -1 ]) usb_c_cutout();

        // LED cutouts
        translate([ 12, .5, INTERNAL_HEIGHT - 8 ]) rotate(90, [ 1, 0, 0 ])
            cylinder(d = LED_DIODE_DIAMETER + .15, h = WALL_THICKNESS + 1, $fn = 32);
        translate([ 20, .5, INTERNAL_HEIGHT - 8 ]) rotate(90, [ 1, 0, 0 ])
            cylinder(d = LED_DIODE_DIAMETER + .15, h = WALL_THICKNESS + 1, $fn = 32);
    }

    // Corners
    translate([ CORNER_INSET_SIZE / 2, CORNER_INSET_SIZE / 2, -WALL_THICKNESS ]) difference()
    {
        cylinder(d = 2 * WALL_THICKNESS + CORNER_INSET_SIZE, h = INTERNAL_HEIGHT + WALL_THICKNESS, $fn = 32);
        translate([ 0, 0, WALL_THICKNESS ]) cylinder(d = CORNER_INSET_SIZE, h = INTERNAL_HEIGHT + 1, $fn = 32);
    }

    translate([ INTERNAL_WIDTH - CORNER_INSET_SIZE / 2, CORNER_INSET_SIZE / 2, -WALL_THICKNESS ]) difference()
    {
        cylinder(d = 2 * WALL_THICKNESS + CORNER_INSET_SIZE, h = INTERNAL_HEIGHT + WALL_THICKNESS, $fn = 32);
        translate([ 0, 0, WALL_THICKNESS ]) cylinder(d = CORNER_INSET_SIZE, h = INTERNAL_HEIGHT + 1, $fn = 32);
    }

    translate([ CORNER_INSET_SIZE / 2, INTERNAL_DEPTH - CORNER_INSET_SIZE / 2, -WALL_THICKNESS ]) difference()
    {
        cylinder(d = 2 * WALL_THICKNESS + CORNER_INSET_SIZE, h = INTERNAL_HEIGHT + WALL_THICKNESS, $fn = 32);
        translate([ 0, 0, WALL_THICKNESS ]) cylinder(d = CORNER_INSET_SIZE, h = INTERNAL_HEIGHT + 1, $fn = 32);
    }

    translate([ INTERNAL_WIDTH - CORNER_INSET_SIZE / 2, INTERNAL_DEPTH - CORNER_INSET_SIZE / 2, -WALL_THICKNESS ])
        difference()
    {
        cylinder(d = 2 * WALL_THICKNESS + CORNER_INSET_SIZE, h = INTERNAL_HEIGHT + WALL_THICKNESS, $fn = 32);
        translate([ 0, 0, WALL_THICKNESS ]) cylinder(d = CORNER_INSET_SIZE, h = INTERNAL_HEIGHT + 1, $fn = 32);
    }

    // Standoffs

    translate([ INTERNAL_WIDTH - 60, INTERNAL_DEPTH - 20, STANDOFF_HEIGHT ]) rotate(90, [ 0, 0, -1 ])
        rpi_pico_standoffs(height = STANDOFF_HEIGHT);

    translate([ INTERNAL_WIDTH - 20, -2, STANDOFF_HEIGHT ]) usb_a_standoffs(height = STANDOFF_HEIGHT);
    translate([ INTERNAL_WIDTH - 45, -2, STANDOFF_HEIGHT ]) usb_a_standoffs(height = STANDOFF_HEIGHT);
    translate([ -1, 20, STANDOFF_HEIGHT ]) rotate(90, [ 0, 0, -1 ]) usb_c_standoffs(height = STANDOFF_HEIGHT);
}

translate([ WALL_THICKNESS, WALL_THICKNESS, WALL_THICKNESS ]) box();

module lid()
{
    difference()
    {
        union()
        {
            translate([ CORNER_INSET_SIZE / 2, -WALL_THICKNESS, 0 ])
                cube([ INTERNAL_WIDTH - CORNER_INSET_SIZE, INTERNAL_DEPTH + 2 * WALL_THICKNESS, WALL_THICKNESS ]);
            translate([ -WALL_THICKNESS, CORNER_INSET_SIZE / 2, 0 ])
                cube([ INTERNAL_WIDTH + 2 * WALL_THICKNESS, INTERNAL_DEPTH - CORNER_INSET_SIZE, WALL_THICKNESS ]);

            translate([ CORNER_INSET_SIZE / 2, CORNER_INSET_SIZE / 2, 0 ])
                cylinder(d = 2 * WALL_THICKNESS + CORNER_INSET_SIZE, h = WALL_THICKNESS, $fn = 32);
            translate([ INTERNAL_WIDTH - CORNER_INSET_SIZE / 2, CORNER_INSET_SIZE / 2, 0 ])
                cylinder(d = 2 * WALL_THICKNESS + CORNER_INSET_SIZE, h = WALL_THICKNESS, $fn = 32);
            translate([ INTERNAL_WIDTH - CORNER_INSET_SIZE / 2, INTERNAL_DEPTH - CORNER_INSET_SIZE / 2, 0 ])
                cylinder(d = 2 * WALL_THICKNESS + CORNER_INSET_SIZE, h = WALL_THICKNESS, $fn = 32);
            translate([ CORNER_INSET_SIZE / 2, INTERNAL_DEPTH - CORNER_INSET_SIZE / 2, 0 ])
                cylinder(d = 2 * WALL_THICKNESS + CORNER_INSET_SIZE, h = WALL_THICKNESS, $fn = 32);
        };

        // Corners
        translate([ CORNER_INSET_SIZE / 2, CORNER_INSET_SIZE / 2, 0 ])
            cylinder(d = CORNER_SCREW_HOLE_SIZE, h = INTERNAL_HEIGHT + 1, $fn = 32);

        translate([ INTERNAL_WIDTH - CORNER_INSET_SIZE / 2, CORNER_INSET_SIZE / 2, 0 ])
            cylinder(d = CORNER_SCREW_HOLE_SIZE, h = INTERNAL_HEIGHT + 1, $fn = 32);

        translate([ CORNER_INSET_SIZE / 2, INTERNAL_DEPTH - CORNER_INSET_SIZE / 2, 0 ])
            cylinder(d = CORNER_SCREW_HOLE_SIZE, h = INTERNAL_HEIGHT + 1, $fn = 32);

        translate([ INTERNAL_WIDTH - CORNER_INSET_SIZE / 2, INTERNAL_DEPTH - CORNER_INSET_SIZE / 2, 0 ])
            cylinder(d = CORNER_SCREW_HOLE_SIZE, h = INTERNAL_HEIGHT + 1, $fn = 32);
    }
}

translate([ WALL_THICKNESS, INTERNAL_DEPTH + 20, 0 ]) lid();

if (SHOW_COMPONENTS)
{
    translate([ WALL_THICKNESS, WALL_THICKNESS, WALL_THICKNESS ])
    {
        translate([ INTERNAL_WIDTH - 60, INTERNAL_DEPTH - 20, STANDOFF_HEIGHT ]) rotate(90, [ 0, 0, -1 ]) rpi_pico();

        translate([ INTERNAL_WIDTH - 20, -2, STANDOFF_HEIGHT ]) usb_a_female_pcb();
        translate([ INTERNAL_WIDTH - 45, -2, STANDOFF_HEIGHT ]) usb_a_female_pcb();

        translate([ -1, 20, STANDOFF_HEIGHT ]) rotate(90, [ 0, 0, -1 ]) usb_c_female_pcb();
    }
}
