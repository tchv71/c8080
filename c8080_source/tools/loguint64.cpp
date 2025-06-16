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

#include "loguint64.h"
#include <limits.h>

uint8_t LogUint64(uint64_t number) {
    switch (number) {
        case 0x0001:
            return 0;
        case 0x0002:
            return 1;
        case 0x0004:
            return 2;
        case 0x0008:
            return 3;
        case 0x0010:
            return 4;
        case 0x0020:
            return 5;
        case 0x0040:
            return 6;
        case 0x0080:
            return 7;
        case 0x0100:
            return 8;
        case 0x0200:
            return 9;
        case 0x0400:
            return 10;
        case 0x0800:
            return 11;
        case 0x1000:
            return 12;
        case 0x2000:
            return 13;
        case 0x4000:
            return 14;
        case 0x8000:
            return 15;
        case 0x10000:
            return 16;
        case 0x20000:
            return 17;
        case 0x40000:
            return 18;
        case 0x80000:
            return 19;
        case 0x100000:
            return 20;
        case 0x200000:
            return 21;
        case 0x400000:
            return 22;
        case 0x800000:
            return 23;
        case 0x1000000:
            return 24;
        case 0x2000000:
            return 25;
        case 0x4000000:
            return 26;
        case 0x8000000:
            return 27;
        case 0x10000000:
            return 28;
        case 0x20000000:
            return 29;
        case 0x40000000:
            return 30;
        case 0x80000000:
            return 31;
        case 0x100000000:
            return 32;
        case 0x200000000:
            return 33;
        case 0x400000000:
            return 34;
        case 0x800000000:
            return 35;
        case 0x1000000000:
            return 36;
        case 0x2000000000:
            return 37;
        case 0x4000000000:
            return 38;
        case 0x8000000000:
            return 39;
        case 0x10000000000:
            return 40;
        case 0x20000000000:
            return 41;
        case 0x40000000000:
            return 42;
        case 0x80000000000:
            return 43;
        case 0x100000000000:
            return 44;
        case 0x200000000000:
            return 45;
        case 0x400000000000:
            return 46;
        case 0x800000000000:
            return 47;
        case 0x1000000000000:
            return 48;
        case 0x2000000000000:
            return 49;
        case 0x4000000000000:
            return 50;
        case 0x8000000000000:
            return 51;
        case 0x10000000000000:
            return 52;
        case 0x20000000000000:
            return 53;
        case 0x40000000000000:
            return 54;
        case 0x80000000000000:
            return 55;
        case 0x100000000000000:
            return 56;
        case 0x200000000000000:
            return 57;
        case 0x400000000000000:
            return 58;
        case 0x800000000000000:
            return 59;
        case 0x1000000000000000:
            return 60;
        case 0x2000000000000000:
            return 61;
        case 0x4000000000000000:
            return 62;
        case 0x8000000000000000:
            return 63;
        // TODO: __builtin_clzll
        default:
            return UINT8_MAX;
    }
}
