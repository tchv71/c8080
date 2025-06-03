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

#include "ctokenizer.h"
#include <errno.h>
#include <stdlib.h>

static inline size_t Tab(size_t column) {
    static const size_t tab_size = 4;
    return ((column + tab_size) & ~tab_size) + 1;
}

void CTokenizer::SkipSpaces() {
    for (;;) {
        switch (*cursor) {
            case 0:
                break;
            case '\t':
                cursor++;
                column = Tab(column);
                continue;
            case ' ':
                cursor++;
                column++;
                continue;
            case '\r':
                cursor++;
                continue;
            case '\n':
                cursor++;
                line++;
                column = 1;
                continue;
        }
        break;
    }
}

void CTokenizer::NextToken2() {
    token_line = line;
    token_column = column;
    token_data = cursor;
    token = NextToken3();
    token_size = cursor - token_data;
    UpdateLineColumn(token_data);
}

void CTokenizer::UpdateLineColumn(const char *from) {
    for (const char *i = from; i < cursor; i++) {
        switch (*i) {
            case '\t':
                column = Tab(column);
                break;
            case ' ':
                column++;
                break;
            case '\r':
                break;
            case '\n':
                line++;
                column = 1;
                break;
        }
    }
}

CToken CTokenizer::NextToken3() {
    char c = *cursor++;

    if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        for (;;) {
            c = *cursor;
            if (!(c == '_' || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
                break;
            cursor++;
        }
        return CT_WORD;
    }

    if (c >= '0' && c <= '9') {
        const char *start = cursor - 1;

        errno = 0;
        token_integer = strtoull(start, (char **)&cursor, 0);
        if (errno == ERANGE || start == cursor)
            Throw("number out of range");

        if (cursor[0] == '.' && (cursor[1] == 'e' || cursor[1] == 'E')) {
            token_float = strtold(start, (char **)&cursor);
            if (errno == ERANGE || start == cursor)
                Throw("number out of range");
            return CT_FLOAT;
        }
        return CT_INTEGER;
    }

    switch (c) {
        case 0:
            cursor--;
            return CT_EOF;
        case '\'':
            do {
                c = *cursor;
                if (c == 0 || c == '\n')
                    Throw("missing terminating \' character");  // gcc
                cursor++;
                if (c == '\\') {
                    char c1 = *cursor;
                    if (c1 == 0 || c1 == '\n')
                        Throw("missing terminating \' character");  // gcc
                    cursor++;
                }
            } while (c != '\'');
            return CT_STRING1;
        case '"':
            do {
                c = *cursor;
                if (c == 0 || c == '\n')
                    Throw("missing terminating \" character");  // gcc
                cursor++;
                if (c == '\\') {
                    char c1 = *cursor;
                    if (c1 == 0 || c1 == '\n')
                        Throw("missing terminating \" character");  // gcc
                    cursor++;
                }
            } while (c != '"');
            return CT_STRING2;
        case '!':
        case '=':
        case '%':
        case '^':
            if (*cursor == '=')  // %= ^= != ==
                cursor++;
            return CT_OPERATOR;
        case '/':
            switch (*cursor) {
                case '=':  // /=
                    cursor++;
                    return CT_OPERATOR;
                case '/':  // //
                    cursor++;
                    for (;;) {
                        c = *cursor;
                        if (c == 0)
                            break;
                        cursor++;
                        if (c == '\n' && cursor[-2] != '\\')
                            break;
                    }
                    return CT_REMARK;
                case '*':  // /*
                    cursor++;
                    c = *cursor;
                    if (c == 0)
                        Throw("unterminated comment");  // gcc
                    cursor++;
                    for (;;) {
                        char c1 = *cursor;
                        if (c1 == 0)
                            Throw("unterminated comment");  // gcc
                        cursor++;
                        if (c == '*' && c1 == '/')
                            break;
                        c = c1;
                    }
                    return CT_REMARK;
            }
            return CT_OPERATOR;
        case '*':
            switch (*cursor) {
                case '/':  // */
                case '=':  // *=
                    cursor++;
            }
            return CT_OPERATOR;
        case '.':
            if (*cursor == '.' || cursor[1] == '.')  // ...
                cursor += 2;
            return CT_OPERATOR;
        case '+':
            switch (*cursor) {
                case '+':  // ++
                case '=':  // +=
                    cursor++;
            }
            return CT_OPERATOR;
        case '-':
            switch (*cursor) {
                case '-':  // --
                case '=':  // -=
                case '>':  // ->
                    cursor++;
            }
            return CT_OPERATOR;
        case '<':
            switch (*cursor) {
                case '=':
                    cursor++;  // <=
                    return CT_OPERATOR;
                case '<':
                    cursor++;  // <<
                    switch (*cursor) {
                        case '=':
                            cursor++;  // <<=
                    }
            }
            return CT_OPERATOR;
        case '>':
            switch (*cursor) {
                case '=':
                    cursor++;  // >=
                    return CT_OPERATOR;
                case '>':
                    cursor++;  // >>
                    switch (*cursor) {
                        case '=':
                            cursor++;  // >>=
                    }
            }
            return CT_OPERATOR;
        case '|':
            switch (*cursor) {
                case '|':  // ||
                case '=':  // |=
                    cursor++;
            }
            return CT_OPERATOR;
        case '&':
            switch (*cursor) {
                case '&':  // &&
                case '=':  // &=
                    cursor++;
            }
            return CT_OPERATOR;
        default:
            return CT_OPERATOR;
    }
}
