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

#include <stdint.h>
#include <c8080/div16mod.h>

uint16_t __div_16_mod;

// Input: hl - value
// Output: hl - result

void __o_minus_16() {
    asm {
        xor  a
        sub  l
        ld   l, a
        ld   a, 0
        sbc  h
        ld  h, a
    }
}

// Input: hl - value 1, de - value 2
// Output: hl - result

void __o_sub_16() {
    asm {
        ld   a, l
        sub  e
        ld   l, a
        ld   a, h
        sbc  d
        ld   h, a
    }
}

// Input: a - value 1, d - value 2
// Output: a - result

void __o_mul_u8() {
    asm {
        ld   hl, 0
        ld   e, d  ; de=d
        ld   d, l
        ld   c, 8
__o_mul_u8__l1:
        add  hl, hl
        add  a
        jp   nc, __o_mul_u8__l2
        add  hl, de
__o_mul_u8__l2:
        dec  c
        jp   nz, __o_mul_u8__l1
        ld   a, l
    }
}

// TODO: __o_mul_i8
// TODO: __o_div_i8
// TODO: __o_div_u8
// TODO: __o_mod_i8
// TODO: __o_mod_u8
// TODO: __o_shl_8
// TODO: __o_shr_u8
// TODO: __o_shr_i8

// Input: hl - value 1, de - value 2
// Output: hl - result

void __o_and_16() {
    asm {
        ld   a, h
        and  d
        ld   h, a
        ld   a, l
        and  e
        ld   l, a
    }
}

// Input: hl - value 1, de - value 2
// Output: hl - result

void __o_or_16() {
    asm {
        ld   a, h
        or   d
        ld   h, a
        ld   a, l
        or   e
        ld   l, a
    }
}

// Input: hl - value 1, de - value 2
// Output: hl - result

void __o_xor_16() {
    asm {
        ld a, h
        xor d
        ld h, a
        ld a, l
        xor e
        ld l, a
        or h     ; Flag Z used for compare
    }
}

// Input: hl - value 1, de - value 2
// Output: hl - result

void __o_mul_u16() {
    asm {
        ld b, h
        ld c, l
        ld hl, 0
        ld a, 17
__o_mul_u16_l1:
        dec a
        ret z
        add hl, hl
        ex hl, de
        add hl, hl
        ex hl, de
        jp nc, __o_mul_u16_l1
        add hl, bc
        jp __o_mul_u16_l1
    }
}

// Input: hl - value 1, de - value 2
// Output: hl - result

void __o_mul_i16() {
    (void)__o_minus_16;
    (void)__o_mul_u16;
    asm {
        ld   a, h
        add  a
        jp   nc, __o_mul_i16_1  ; hl - positive

        call __o_minus_16

        ld   a, d
        add  a
        jp   nc, __o_mul_i16_2  ; hl - negative, de - positive

        ex   hl, de
        call __o_minus_16
        ex   hl, de

        jp   __o_mul_u16 ; hl & de - negative

__o_mul_i16_1:
        ld   a, d
        add  a
        jp   nc, __o_mul_u16  ; hl & de - positive

        ex   hl, de
        call __o_minus_16
        ex   hl, de

__o_mul_i16_2:
        call __o_mul_u16
        jp   __o_minus_16
    }
}

// Input: hl - value 1, de - value 2
// Output: hl - result, de - result, __div_16_mod

void __o_div_u16() {
    (void)__div_16_mod;
    asm {
        call __o_div_u16__l0
        ex hl, de
        ld (__div_16_mod), hl
        ex hl, de
        ret

__o_div_u16__l0:
        ex hl, de
__o_div_u16__l:
        ld a, h
        or l
        ret z
        ld bc, 0
        push bc
__o_div_u16__l1:
        ld a, e
        sub l
        ld a, d
        sbc h
        jp c, __o_div_u16__l2
        push hl
        add hl, hl
        jp nc, __o_div_u16__l1
__o_div_u16__l2:
        ld hl, 0
__o_div_u16__l3:
        pop bc
        ld a, b
        or c
        ret z
        add hl, hl
        push de
        ld a, e
        sub c
        ld e, a
        ld a, d
        sbc b
        ld d, a
        jp c, __o_div_u16__l4
        inc hl
        pop bc
        jp __o_div_u16__l3
__o_div_u16__l4:
        pop de
        jp __o_div_u16__l3
    }
}

// Input: hl - value 1, de - value 2
// Output: hl - result, de - result, __div_16_mod

void __o_div_i16() {
    (void)__o_minus_16;
    (void)__o_div_u16;
    asm {
        ld   a, h
        add  a
        jp   nc, __o_div_i16_1  ; hl - positive

        call __o_minus_16

        ld   a, d
        add  a
        jp   nc, __o_div_i16_2  ; hl - negative, de - positive

        ex   hl, de
        call __o_minus_16
        ex   hl, de

        jp   __o_div_u16 ; hl & de - negative

__o_div_i16_1:
        ld   a, d
        add  a
        jp   nc, __o_div_u16  ; hl & de - positive

        ex   hl, de
        call __o_minus_16
        ex   hl, de

__o_div_i16_2:
        call __o_div_u16
        jp   __o_minus_16
    }
}

// Input: hl - value 1, de - value 2
// Output: hl - result, de - result, __div_16_mod

void __o_mod_u16() {
    (void)__o_div_u16;
    asm {
        call __o_div_u16__l0
        ex hl, de
    }
}

// TODO:  __o_mod_i16

// Input: hl - value 1, de - value 2
// Output: hl - result

void __o_shl_16() {
    asm {
        inc  e
__o_shl_16__l1:
        dec  e
        ret  z
        add  hl, hl
        jp   __o_shl_16__l1
    }
}
