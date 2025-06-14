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

#include "compile.h"
#include "8080_compile.h"
#include "8080_prepare.h"

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
    // TODO: Зарезервированные имена ассемблера
}

void Compile_8080(CParser &c, CProgramm &programm, OutputFormat output_format, const std::string &output_file_bin,
                  CString asm_file_name) {
    Compile8080 compile8080;
    compile8080.Compile(c, programm, output_format, output_file_bin, asm_file_name);
}

bool ParseOutputFormat(enum OutputFormat &result, const char *str) {
    if (0 == strcasecmp(str, "i1080")) {
        result = OF_I1080;
        return true;
    }
    if (0 == strcasecmp(str, "cpm")) {
        result = OF_CPM;
        return true;
    }
    return false;
}
