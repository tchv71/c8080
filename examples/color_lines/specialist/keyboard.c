/*
 * Keyboard driver with codepage 866 for 8-bit Computer Specialist
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

#include "../hal.h"

#define KEYB0 (*(uint8_t *)0xF800)
#define KEYB1 (*(uint8_t *)0xF801)
#define KEYB2 (*(uint8_t *)0xF802)
#define KEYB3 (*(uint8_t *)0xF803)

static const uint8_t scanCodes[1] = {
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,

    0x01,   0x0C,   0x17,   0x1A,   0x09,   0x1B,   ' ',    0x19,   0x02,   0x18,    0x0A,    KEY_ENTER,

    ';',    '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',     '0',     '-',
    'j',    'c',    'u',    'k',    'e',    'n',    'g',    '[',    ']',    'z',     'h',     '*',
    'f',    'y',    'w',    'a',    'p',    'r',    'o',    'l',    'd',    'v',     '\\',    '.',
    'q',    '^',    's',    'm',    'i',    't',    'x',    'b',    '@',    ',',     '/',     8,

    '+',    '!',    '"',    '#',    '$',    '%',    '^',    '&',    '*',    '(',     ')',     '=',
    'J',    'C',    'U',    'K',    'E',    'N',    'G',    '{',    '}',    'Z',     'H',     ':',
    'F',    'Y',    'W',    'A',    'P',    'R',    'O',    'L',    'D',    'V',     '\\',    '>',
    'Q',    '^',    'S',    'M',    'I',    'T',    'X',    'B',    '@',    '<',     '?',     8,

    ';',    '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',     '0',     '-',
    'й',    'ц',    'у',    'к',    'е',    'н',    'г',    'ш',    'щ',    'з',     'х',     '*',
    'ф',    'ы',    'в',    'а',    'п',    'р',    'о',    'л',    'д',    'ж',     'э',     '.',
    'я',    'ч',    'с',    'м',    'и',    'т',    'ь',    'б',    'ю',    ',',     '/',     8,

    '+',    '!',    '"',    '#',    '$',    '%',    '^',    '&',    '*',    '(',     ')',     '=',
    'Й',    'Ц',    'У',    'К',    'Е',    'Н',    'Г',    'Ш',    'Щ',    'З',     'Х',     ':',
    'Ф',    'Ы',    'В',    'А',    'П',    'Р',    'О',    'Л',    'Д',    'Ж',     'Э',     '>',
    'Я',    'Ч',    'С',    'М',    'И',    'Т',    'Ь',    'Б',    'Ю',    '<',     '?',     8,
};

// 0x80 - CTRL

uint8_t numberOfBit(uint8_t b) {
    asm {
        ld   e, 7
        ld   d, 7
numberOfBit_l:
        rra
        jp   nc, numberOfBit_o
        dec  e
        dec  d
        jp   nz, numberOfBit_l
numberOfBit_o:
        ld   a, e
    }
}

static uint8_t rus, prevCh;

static const uint8_t scan[6] = {0x80 ^ ~3, 0x04 ^ ~3, 0x40 ^ ~3, 0x20 ^ ~3, 0x10 ^ ~3, 0x08 ^ ~3};

bool IsShiftPressed(void) {
    KEYB3 = 0x82;
    return ~KEYB1 & 2;
}

uint8_t ReadKeyboard(bool noWait) {
    uint8_t i, u, b;

    KEYB3 = 0x91;

retry:
    for (;;) {
        i = 6;
        for (;;) {
            --i;
            KEYB1 = scan[i];
            b = KEYB0;
            if (b != 0xFF) {
                u = 4;
                break;
            }
            b = KEYB2 | 0xF0;
            if (b != 0xFF) {
                u = -4;
                break;
            }
            if (i)
                continue;
            i = 6;
            prevCh = -1;
            if (noWait)
                return 0;
        }
        b = numberOfBit(b) + u + i * 12;
        if (noWait)
            break;
        if (prevCh != b)
            break;
    }

    prevCh = b;

    if (b == 12) {
        rus = !rus;
        goto retry;
    }

    if (b >= 24) {
        if (IsShiftPressed())
            b += 12 * 4;
        if (rus)
            b += 12 * 8;
    }

    return scanCodes[b];
}
