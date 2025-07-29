/*
 * Fillrect function for 8-bit Computer Specialist
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

#include "fillrect.h"

void FillRectOver(void) {
    (void)FillRectTrue;
    asm {
        ld   a, 0AAh ; XOR D
        jp   fillrecttrue_1
    }
}

void FillRectInverse(void) {
    (void)FillRectTrue;
    asm {
        ld   a, 0A2h ; AND D
        ld   (fillrect_int_cmd), a
        ld   a, 02Fh ; CMA
        jp   fillrecttrue_2
    }
}

void FillRectTrue(void) {
    asm {
        ld   a, 0B2h ; OR D
fillrecttrue_1:
        ld   (fillrect_int_cmd), a
        xor  a ; NOP
fillrecttrue_2:
        ld   (fillrect_int_cmd2), a
    }
}

void __global FillRectInt(uint8_t len, uint8_t x, uint8_t *a) {
    asm {
__a_3_fillrectint=0
__a_2_fillrectint=$+1
        ld   a, 0
fillrect_int_cmd2:
        nop          ; CMA = 2F NOP = 00
        ld   d, a
__a_1_fillrectint=$+1
        ld   e, 0
fillrect_int_l0:
        ld   a, (hl)
fillrect_int_cmd:
        or   d        ; XOR D = AA, AND D = A2, OR D = B2
        ld   (hl), a
        inc  l
        dec  e
        jp   nz, fillrect_int_l0
    }
}

void FillRect(uint8_t *tile, uint16_t centerSize, uint8_t leftMask, uint8_t rightMask, uint8_t height) {
    if (centerSize == 0) {
        FillRectInt(height, leftMask & rightMask, tile);
        return;
    }
    --centerSize;
    FillRectInt(height, leftMask, tile);
    tile += 0x100;
    for (; centerSize; --centerSize) {
        FillRectInt(height, 0xFF, tile);
        tile += 0x100;
    }
    FillRectInt(height, rightMask, tile);
}

void FillRectXY(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1) {
    FillRect(FILLRECTARGS(x0, y0, x1, y1));
}
