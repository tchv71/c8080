/*
 * Draw text function for 8-bit Computer Specialist
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

#include "drawtext.h"
#include <hal/hal.h>

extern uint8_t chargen[2048];

void DrawTextXor(void) {
    (void)DrawTextNormal;
    asm {
        ld   hl, 0FEh ; NOP + CPI
        jp   drawTextNormal_1
    }
}

void DrawTextInverse(void) {
    (void)DrawTextNormal;
    asm {
        ld   hl, 02FE6h ; CMA + AND
        jp   drawTextNormal_1
    }
}

void DrawTextNormal(void) {
    asm {
        ld   hl, 0E6h ; NOP + AND
drawTextNormal_1:
        ld   a, l
        ld   (print_mode1), a
        ld   (print_mode2), a
        ld   (print_mode3), a
        ld   (print_mode4), a
        ld   (print_mode5), a
        ld   (print_mode6), a
        ld   a, h
        ld   (print_mode1A), a
        ld   (print_mode2A), a
        ld   (print_mode3A), a
        ld   (print_mode4A), a
        ld   (print_mode5A), a
    }
}

static void __global print_p1(void *, void *) {
    asm {
__a_2_print_p1=__a_2_print_p1
        ex   hl, de
__a_1_print_p1=$+1
        ld   hl, 0
        ld   c,  8
print_p1_1:
        ld   a,  (de)
print_mode1A:
        nop
        add  a
        add  a
        ld   b, a
        ld   a, (hl)
print_mode1:
        and  3
        xor  b
        ld   (hl), a
        inc  de
        inc  hl
        dec  c
        jp   nz, print_p1_1
    }
}

static void __global print_p2(void *, void *) {
    asm {
__a_2_print_p2=__a_2_print_p2
        ex   hl, de
__a_1_print_p2=$+1
        ld   hl, 0
        ld   c, 8
print_p2_1:
        ld   a, (de)
print_mode2A:
        nop
        add  a
        adc  a
        adc  a
        adc  a
        push af
        adc  a
        and  00000011b
        ld   b, a
        ld   a, (hl)
print_mode2:
        and  0FCh
        xor  b
        ld   (hl), a

        inc  h

        pop  af
        and  011110000b
        ld   b, a
        ld   a, (hl)
print_mode3:
        and  00Fh
        xor  b
        ld   (hl), a

        dec  h

        inc  de
        inc  hl
        dec  c
        jp   nz, print_p2_1
    }
}

static void __global print_p3(void *, void *) {
    asm {
__a_2_print_p3=__a_2_print_p3
        ex   hl, de
__a_1_print_p3=$+1
        ld   hl, 0
        ld   c,  8
print_p3_1:
        ld   a, (de)
print_mode3A:
        nop
        rra
        rra
        and  00001111b
        ld   b, a
        ld   a, (hl)
print_mode4:
        and  0F0h
        xor  B
        ld   (hl), A

        inc  h

        ld   a, (de)
print_mode4A:
        nop
        rra
        rra
        rra
        and  11000000b
        ld   b, a
        ld   a, (hl);
print_mode5:
        and  03Fh
        xor  b
        ld   (hl), a

        dec  h

        inc de
        inc hl
        dec c
        jp  nz, print_p3_1
    }
}

static void __global print_p4(void *, void *) {
    asm {
__a_2_print_p4=__a_2_print_p4
        ex   hl, de
__a_1_print_p4=$+1
        ld   hl, 0
        ld   c,  8
print_p4_1:
        ld   a, (de)
print_mode5A:
        nop
        and  03Fh
        ld   b, a
        ld   a, (hl)
print_mode6:
        and  0C0h
        xor  b
        ld   (hl), a
        inc  de
        inc  hl
        dec  c
        jp   nz, print_p4_1
    }
}

void DrawText(uint8_t *tile, uint8_t pixelOffset, const char *text) {
    for (;;) {
        uint8_t c = *text;
        if (c == 0)
            return;
        uint8_t *s = chargen + c * 8;
        switch (pixelOffset) {
            case 0:
                print_p1(tile, s);
                pixelOffset = 3;
                break;
            case 1:
                print_p4(tile, s);
                pixelOffset = 0;
                tile += 0x100;
                break;
            case 2:
                print_p3(tile, s);
                pixelOffset = 1;
                tile += 0x100;
                break;
            case 3:
                print_p2(tile, s);
                pixelOffset = 2;
                tile += 0x100;
                break;
        }
        text++;
    }
}

void DrawTextXY(uint8_t x, uint8_t y, const char *text) {
    DrawText(DRAWTEXTARGS(x, y), text);
}
