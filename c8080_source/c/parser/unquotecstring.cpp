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

#include "unquotecstring.h"

const char *UnquoteCString(std::string &str) {
    char *begin = &str[0];
    char *dest = begin;
    const char *src = dest + 1;
    const char *src_end = dest + str.size() - 1;

    if (str.size() < 2 || (*begin != '"' && *begin != '\'') || *begin != *src_end)
        return "string not quoted";

    while (src != src_end) {
        char c = *src++;
        if (c == '\\') {
            if (src == src_end)
                return "unknown ESC sequence";
            c = *src++;
            switch (c) {
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case '\\':
                    c = '\\';
                    break;
                case '\'':
                    c = '\'';
                    break;
                case '"':
                    c = '"';
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7': {
                    src--;
                    char *end = nullptr;
                    const unsigned long long result = strtoull(src, &end, 8);
                    if (result > UINT8_MAX || src == end)
                        return "octal escape sequence out of range";  // gcc
                    src = end;
                    c = uint8_t(result);
                    break;
                }
                case 'x': {
                    char *end = nullptr;
                    const unsigned long long result = strtoull(src, &end, 16);
                    if (src == end)
                        return "\\x used with no following hex digits";  // gcc
                    if (result > UINT8_MAX)
                        return "hex escape sequence out of range";  // gcc
                    src = end;
                    c = uint8_t(result);
                    break;
                }
                default:
                    return "unknown ESC sequence";
            }
        }
        *dest++ = c;
    }
    str.resize(dest - begin);
    return nullptr;
}
