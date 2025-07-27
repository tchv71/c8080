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

void __global Sound(uint8_t period, uint16_t count) {
    asm {
__a_2_sound=__a_2_sound
        ex   hl, de
        ld   hl, 0F802h
__a_1_sound=$+1
        ld   b, 0
        dec  b           ; Compensating for extra ticks
        dec  b           ; 10+5 + 5+5+10 + 5+5+5+10 = 60/30 = 2
Sound1: ld   (hl), 0     ; extra 10
        ld   a, b        ; extra 5
Sound2: dec  a           ; 5
        jp   nz, Sound2  ; 10
        dec  de          ; extra 5
        ld   a, b        ; extra 5
        ld   (hl), 20h   ; extra 10
Sound3: dec  a           ; 5
        jp   nz, Sound3  ; 10
        ld   c, d        ; extra 5
        ld   c, d        ; extra 5
        inc  c           ; extra 5
        jp   nz, Sound1  ; extra 10
    }
}
