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
#include <assert.h>

namespace I8080 {

void RegisterProhibitedOutputNames(CProgramm &programm) {
    static const char *prohibited_output_names[] = {
        // These names conflicts with static headers

        "__begin",
        "__entry",
        "__bss",
        "__end",

        // These names conflicts with the assembler
        // Example:
        //      ld a, (a)
        //   a: db 10

        "a",
        "b",
        "c",
        "d",
        "e",
        "h",
        "l",
        "h",
        "i",
        "r",
        "bc",
        "de",
        "hl",
        "sp",
        "ix",
        "iy",
        "ixl",
        "iyl",
    };

    for (auto name : prohibited_output_names) {
        const bool result = programm.AddOutputName(name);
        assert(result);
    }
}

}  // namespace I8080
