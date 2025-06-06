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

#include "utf8to8bit.h"
#include "parseutf8.h"

size_t Utf8To8Bit(const std::map<uint32_t, uint8_t> &codepage, CString utf8, std::string &result) {
    const size_t size = utf8.size();
    const char *utf8_begin = utf8.data();
    const char *utf8_iterator = utf8_begin;
    const char *utf8_end = utf8_begin + utf8.size();

    result.resize(size);
    char *result_begin = result.data();
    char *result_iterator = result_begin;

    while (utf8_iterator != utf8_end) {
        uint32_t unicode_char;
        if (!ParseUtf8(unicode_char, utf8_iterator, utf8_end)) {
            result.clear();
            return result_iterator - result_begin;
        }
        auto i = codepage.find(unicode_char);
        if (i != codepage.end()) {
            *result_iterator++ = char(i->second);
        } else if (unicode_char < 128) {
            *result_iterator++ = char(unicode_char);
        } else {
            result.clear();
            return result_iterator - result_begin;
        }
    }
    result.resize(result_iterator - result_begin);
    return SIZE_MAX;
}
