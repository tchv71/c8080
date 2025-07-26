/*
 * c8080 stdlib
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

#include "micro80.h"

void DrawText(void *tile, uint8_t color, const char *text) {
    asm {
__a_3_drawtext=__a_3_drawtext
        ex   hl, de
__a_1_drawtext=$+1
        ld   bc, 0 ; tile
        ld   a, b
        sub  08h
        ld   h, a
        ld   l, c
drawtext_l1:
        ld   a, (de)
        or   a
        ret  z
        ld   (bc), a
__a_2_drawtext=$+1
        ld   (hl), 0 ; color
        inc  c
        inc  l
        inc  de
        jp   drawtext_l1
    }
}
