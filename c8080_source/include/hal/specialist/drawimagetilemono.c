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

void __global DrawImageTileMono(void* tile, const void* image, uint16_t width_height) {
    asm {
__a_3_drawimagetilemono=__a_3_drawimagetilemono
        ld   bc, hl
__a_1_drawimagetilemono=$+1
        ld   de, 0
__a_2_drawimagetilemono=$+1
        ld   hl, 0
DrawImageTileMono_1:
        push de
        push bc
DrawImageTileMono_2:
        ld   a, (hl)
        inc  hl
        ld   (de), a
        inc  de
        dec  c
        jp   nz, DrawImageTileMono_2
        pop  bc
        pop  de
        inc  d
        dec  b
        jp   nz, DrawImageTileMono_1
    }
}
