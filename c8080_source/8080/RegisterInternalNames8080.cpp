/*
 * c8080 compiler
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Compile.h"

void RegisterInternalNames8080(CProgramm &programm) {
    programm.output_names["__begin"] = 1;
    programm.output_names["__entry"] = 1;
    programm.output_names["__bss"] = 1;
    programm.output_names["__end"] = 1;

    programm.output_names["a"] = 1;  // Cannot be a variable name: ld a, (a)
    programm.output_names["b"] = 1;
    programm.output_names["c"] = 1;
    programm.output_names["d"] = 1;
    programm.output_names["e"] = 1;
    programm.output_names["h"] = 1;
    programm.output_names["l"] = 1;
    programm.output_names["h"] = 1;
    programm.output_names["i"] = 1;
    programm.output_names["r"] = 1;
    programm.output_names["bc"] = 1;
    programm.output_names["de"] = 1;
    programm.output_names["hl"] = 1;
    programm.output_names["sp"] = 1;
    programm.output_names["ix"] = 1;
    programm.output_names["iy"] = 1;
    programm.output_names["ixl"] = 1;
    programm.output_names["iyl"] = 1;
}
