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

#include "ccomfortablelex.h"
#include "cdecodestring.h"

bool CComfortableLex::IfString1(std::string &out_string) {
    if (token != CT_STRING1)
        return false;
    out_string.assign(token_data, token_size);
    const char *error = CDecodeString(out_string);
    if (error)
        Throw(error);
    NextToken();
    return true;
}

bool CComfortableLex::IfString2(std::string &out_string) {
    if (token != CT_STRING2)
        return false;
    out_string.assign(token_data, token_size);
    NextToken();
    while (token == CT_STRING2) {
        out_string.append(token_data, token_size);
        NextToken();
    }
    const char *error = CDecodeString(out_string);
    if (error)
        Throw(error);
    return true;
}

bool CComfortableLex::IfToken(const char *const *strings, size_t &out_index) {
    for (auto i = strings; *i != nullptr; i++) {
        if (IfToken(*i)) {
            out_index = i - strings;
            return true;
        }
    }
    return false;
}

bool CComfortableLex::IfToken(const std::vector<std::string> &strings, size_t &out_index) {
    for (auto i = strings.begin(), i_end = strings.end(); i != i_end; i++) {
        if (IfToken(*i)) {
            out_index = i - strings.begin();
            return true;
        }
    }
    return false;
}
