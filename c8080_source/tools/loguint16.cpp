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

#include "loguint16.h"
#include <limits.h>

uint16_t LogUint16(uint16_t number) {
    switch (number) {
        case 0x0001:
            return 0u;
        case 0x0002:
            return 1u;
        case 0x0004:
            return 2u;
        case 0x0008:
            return 3u;
        case 0x0010:
            return 4u;
        case 0x0020:
            return 5u;
        case 0x0040:
            return 6u;
        case 0x0080:
            return 7u;
        case 0x0100:
            return 8u;
        case 0x0200:
            return 9u;
        case 0x0400:
            return 10u;
        case 0x0800:
            return 11u;
        case 0x1000:
            return 12u;
        case 0x2000:
            return 13u;
        case 0x4000:
            return 14u;
        case 0x8000:
            return 15u;
        default:
            return UINT16_MAX;
    }
}
