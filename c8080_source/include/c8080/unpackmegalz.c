/*
 * MegaLZ Depacker
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

#include "unpackmegalz.h"

void* __global UnpackMegalz(void* destination, const void* source) {
    asm {
__a_2_unpackmegalz=__a_2_unpackmegalz
        ex   hl, de  ; source
__a_1_unpackmegalz=$+1
        ld   bc, 0  ; destination
        ld   a, 0x80
UnpackMegalz_0:
        ld   (UnpackMegalz_5 + 1), a
        ld   a, (de)
        inc  de
        jp   UnpackMegalz_4
UnpackMegalz_1:
        ld   a, (hl)
        inc  hl
        ld   (bc), a
        inc  bc
UnpackMegalz_2:
        ld   a, (hl)
        inc  hl
        ld   (bc), a
        inc  bc
UnpackMegalz_3:
        ld   a, (hl)
UnpackMegalz_4:
        ld   (bc), a
        inc  bc
UnpackMegalz_5:
        ld   a, 0x80
        add  a
        call z, UnpackMegalz_20
        jp   c, UnpackMegalz_0
        add  a
        call z, UnpackMegalz_20
        jp   c, UnpackMegalz_7
        add  a
        call z, UnpackMegalz_20
        jp   c, UnpackMegalz_6
        ld   hl, 16383
        call UnpackMegalz_16
        ld   (UnpackMegalz_5 + 1), a
        add  hl, bc
        jp   UnpackMegalz_3

UnpackMegalz_6:
        ld   (UnpackMegalz_5 + 1), a
        ld   a, (de)
        inc  de
        ld   l, a
        ld   h, 255
        add  hl, bc
        jp   UnpackMegalz_2

UnpackMegalz_7:
        add  a
        call z, UnpackMegalz_20
        jp   c, UnpackMegalz_8
        call UnpackMegalz_18
        add  hl, bc
        jp   UnpackMegalz_1

UnpackMegalz_8:
        ld   h, 0
UnpackMegalz_9:
        inc  h
        add  a
        call z, UnpackMegalz_20
        jp   nc, UnpackMegalz_9
UnpackMegalz_10:
        push af
        ld   a, h
        cp   8
        jp   nc, UnpackMegalz_15
        ld   a, 0
UnpackMegalz_11:
        rra
        dec  h
        jp   nz, UnpackMegalz_11
UnpackMegalz_12:
        ld   h, a
        ld   l, 1
        pop  af
        call UnpackMegalz_16
        inc  hl
        inc  hl
        push hl
        call UnpackMegalz_18
        ex de, hl
        ex   (sp), hl
        ex de, hl
        add  hl, bc
UnpackMegalz_13:
        ld   a, (hl)
        inc  hl
        ld   (bc), a
        inc  bc
        dec  e
        jp   nz, UnpackMegalz_13
UnpackMegalz_14:
        pop  de
        jp   UnpackMegalz_5
UnpackMegalz_15:
        pop  af
        ex   de, hl
        ret

UnpackMegalz_16:
        add  a
        call z, UnpackMegalz_20
        jp   c, UnpackMegalz_17
        add  hl, hl
        ret  c
        jp UnpackMegalz_16
UnpackMegalz_17:
        add  hl, hl
        inc  l
        ret  c
        jp   UnpackMegalz_16

UnpackMegalz_18:
        add  a
        call z, UnpackMegalz_20
        jp   c, UnpackMegalz_19
        ld   (UnpackMegalz_5 + 1), a
        ld   a, (de)
        inc  de
        ld   l, a
        ld   h, 255
        ret

UnpackMegalz_19:
        ld   hl, 8191
        call UnpackMegalz_16
        ld   (UnpackMegalz_5 + 1), a
        ld   h, l
        dec  h
        ld   a, (de)
        inc  de
        ld   l, a
        ret

UnpackMegalz_20:
        ld   a, (de)
        inc  de
        rla
    }
}
