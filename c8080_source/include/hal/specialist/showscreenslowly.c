/* c8080 stdlib
 * Copyright (c) 2022 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include "specialist.h"

void ShowScreenSlowly(void) {
    asm {
        ld   b, 0
        ld   e, 0
ShowScreenSlowly_1:
        ld   a, e
        add  a
        add  a
        add  e
        inc  a
        ld   e, a
        ld   d, 48h
        ld   h, 90h
        ld   l, e
        ld   c, 48
ShowScreenSlowly_2:
        ld   a, (de)
        inc  d
        ld   (0F802h), a
        ld   a, (hl)
        ld   (hl), a
        inc  h
        dec  c
        jp   nz,  ShowScreenSlowly_2
        dec  b
        jp   nz,  ShowScreenSlowly_1
    }
}

