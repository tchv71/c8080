#include "graph_functions.h"
#include <string.h>
#include <stdbool.h>
#include <hal/hal.h>

void graphXor(void) {
    (void)graph1;
    asm {
        ld   hl, 0FEh ; CPI
        ld   a, 0AAh
        jp   graph1_l1
    }
}

void graph0(void) {
    (void)graph1;
    asm {
        ld   hl, 0E6h ; ANI
        ld   a, 0A2h
        ld   (fillRect1_int_cmd), a
        ld   a, 02Fh
        jp   graph1_l2
    }
}

void graph1(void) {
    asm {
        ld   hl, 02FE6h ; ORI !
        ld   a, 0B2h
graph1_l1:
        ld   (fillRect1_int_cmd), a
        xor  a
graph1_l2:
        ld   (fillRect1_int_cmd2), a
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

#ifdef XXX
void clrscr10(void *t, uint8_t w, uint8_t h) {
    asm {
        ld   hl, 0
        dad  sp
        ld   (clrscr10_saveHl), hl
__a_3_strlen=__a_3_strlen
__a_2_strlen=$+1
        ld   c, 0
__a_1_strlen=$+1
        ld   hl, 0
        dec  hl
        ld   de, 0
clrscr10_l2:
        sphl
        ld   b, a
clrscr10_l1:
        push de
        push de
        push de
        push de
        push de
        dec  b
        jp   nz, clrscr10_l1
        inc  h
        dec  c
        jp   nz, clrscr10_l2
clrscr10_saveHl=$+1
        ld   hl, 0
        sphl
    }
}
#endif

void fillRect1int(uint8_t len, uint8_t x, uint8_t *a) {
    asm {
__a_3_fillrect1int=__a_3_fillrect1int
__a_2_fillrect1int=$+1
        ld   a, 0
fillRect1_int_cmd2:
        nop          ; CMA = 2F NOP = 00
        ld   d, a
__a_1_fillrect1int=$+1
        ld   e, 0
fillRect1_int_l0:
        ld   a, (hl)
fillRect1_int_cmd:
        or   d        ; XRA D = AA, ANA D = A2, ORA D = B2
        ld   (hl), a
        inc  l
        dec  e
        jp   nz, fillRect1_int_l0
    }
}

void FillRectFast(uint8_t *a, uint16_t c, uint8_t l, uint8_t r, uint8_t h) {
    if (c == 0) {
        fillRect1int(h, l & r, a);
        return;
    }
    --c;
    fillRect1int(h, l, a);
    a += 0x100;
    for (; c; --c) {
        fillRect1int(h, 0xFF, a);
        a += 0x100;
    }
    fillRect1int(h, r, a);
}

void FillRect(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1) {
    FillRectFast(FILLRECTARGS(x0, y0, x1, y1));
}

extern uint8_t chargen[2048];

void print_p1(void *, void *) {
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

void print_p2(void *, void *) {
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

void print_p3(void *, void *) {
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

void print_p4(void *, void *) {
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

void DrawText(uint8_t *tile, uint8_t pixelOffset, uint8_t color, const char *text) {
    SET_COLOR(color);
    for (;;) {
        uint8_t c = *text;
        if (c == 0)
            return;
        uint8_t *s = chargen + c * 8;
        switch (pixelOffset) {
            case 0:
                print_p1(tile, s);
                ++pixelOffset;
                break;
            case 1:
                print_p2(tile, s);
                ++pixelOffset;
                tile += 0x100;
                break;
            case 2:
                print_p3(tile, s);
                ++pixelOffset;
                tile += 0x100;
                break;
            case 3:
                print_p4(tile, s);
                pixelOffset = 0;
                tile += 0x100;
                break;
        }
        text++;
    }
}

void DrawTextXY(uint8_t x, uint8_t y, uint8_t color, const char *text) {
    DrawText(PRINTARGS(x, y), color, text);
}
