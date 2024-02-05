# Copyright (C) 2024 Oliver Larsson
#
# This file is part of si-printer-bridge.
#
# si-printer-bridge is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# si-printer-bridge is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with

OUTFILE = enclosure.stl

.PHONY: all clean

all: ${OUTFILE}

${OUTFILE}: enclosure.scad config.scad modules/*.scad 
	openscad -o $@ $<

clean: 
	rm -f ${OUTFILE}
