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

#include "convertutf8.h"

bool ConvertUtf8(uint32_t &output, const char *&input, const char *input_end) {
    if (input == input_end)
        return false;

    const uint8_t a = *input++;
    if ((a & 0x80) == 0) { /* 0xxxxxxx */
        output = a;
        return 1;
    }

    if ((a & 0xE0) == 0xC0) { /* 110xxxxx */
        if (input == input_end)
            return false;
        const uint8_t b = *input++;
        if ((b & 0xC0) != 0x80) /* 10xxxxxx */
            return false;
        output = ((a & 0x1F) << 6) | (b & 0x3F);
        return true;
    }

    if ((a & 0xF0) == 0xE0) { /* 1110xxxx */
        if (input_end - input < 2)
            return false;
        const uint8_t b = *input++;
        if ((b & 0xC0) != 0x80) /* 10xxxxxx */
            return false;
        const uint8_t c = *input++;
        if ((c & 0xC0) != 0x80) /* 10xxxxxx */
            return false;
        output = ((a & 0x0F) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
        return true;
    }

    if ((a & 0xF8) == 0xF0) { /* 11110xxx */
        if (input_end - input < 3)
            return false;
        const uint8_t b = *input++;
        if ((b & 0xC0) != 0x80) /* 10xxxxxx */
            return false;
        const uint8_t c = *input++;
        if ((c & 0xC0) != 0x80) /* 10xxxxxx */
            return false;
        const uint8_t d = *input++;
        if ((d & 0xC0) != 0x80) /* 10xxxxxx */
            return false;
        output = ((a & 0x07) << 18) | ((b & 0x3F) << 12) | ((c & 0x3F) << 6) | (d & 0x3F);
        return true;
    }

    return false;
}
