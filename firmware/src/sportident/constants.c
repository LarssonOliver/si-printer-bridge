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

#include "constants.h"

const si_card_def_t SI5 = {
    6,      4,      5,  F_NONE, F_NONE, 19,     F_NONE, F_NONE, 21,
    F_NONE, F_NONE, 25, F_NONE, F_NONE, F_NONE, 23,     32,     3,
    30,     F_NONE, 0,  1,      2,      F_NONE, 1,
};

const si_card_def_t SI6 = {
    11, 12, 13, 24,  25, 26, 20, 21, 22, 28, 29,     30, 32,
    33, 34, 18, 128, 4,  64, 0,  1,  2,  3,  F_NONE, 3,
};

const si_card_def_t SI8 = {
    25,     26,     27, 12,  13, 14, 16, 17, 18, 8, 9, 10, F_NONE,
    F_NONE, F_NONE, 22, 136, 4,  50, 0,  1,  2,  3, 2, 2,
};

const si_card_def_t SI9 = {
    25,     26,     27, 12, 13, 14, 16, 17, 18, 8, 9, 10, F_NONE,
    F_NONE, F_NONE, 22, 56, 4,  50, 0,  1,  2,  3, 2, 2,
};

const si_card_def_t pCard = {
    25,     26,     27, 12,  13, 14, 16, 17, 18, 8, 9, 10, F_NONE,
    F_NONE, F_NONE, 22, 176, 4,  20, 0,  1,  2,  3, 2, 2,
};

// Also SI11
const si_card_def_t SI10 = {
    25,     26,     27, 12,  13, 14, 16, 17, 18, 8, 9, 10, F_NONE,
    F_NONE, F_NONE, 22, 128, 4,  64, 0,  1,  2,  3, 8, 5,
};
