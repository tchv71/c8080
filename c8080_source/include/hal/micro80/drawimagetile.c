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

void DrawImageTile(void *tile, const void *image, uint16_t width_height) {
    asm {
__a_3_drawimagetile=__a_3_drawimagetile
        ld   bc, hl ; width, height
__a_1_drawimagetile=$+1
        ld   hl, 0 ; tile
__a_2_drawimagetile=$+1
        ld   de, 0 ; image
drawimagetile_l1:
        push bc
        push hl
drawimagetile_l2:
        ld   a, (de)
        inc  de
        ld   (hl), a

        ld   a, h
        sub  08h
        ld   h, a

        ld   a, (de)
        inc  de
        ld   (hl), a

        ld   a, h
        add  08h
        ld   h, a

        inc  l

        dec  c
        jp   nz, drawimagetile_l2

        pop  hl

        ld   bc, 64
        add  hl, bc

        pop  bc

        dec  b
        jp   nz, drawimagetile_l1
    }
}
