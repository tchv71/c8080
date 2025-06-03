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

#pragma once

#include <map>
#include <functional>
#include <list>
#include <string>
#include <stdexcept>
#include <string.h>
#include "../../tools/cstring.h"
#include "../cerrorposition.h"
#include "cdecodestring.h"
#include "ctokenizer.h"

class CLex : public CTokenizer {
private:
    struct Macro {
        const char *name{};
        size_t name_size{};
        const char *body{};
        bool disabled{};  // Macro should not call itself
        std::vector<std::string> args;
    };

    struct Stack {
        const char *cursor{};
        size_t line{};
        size_t column{};
        size_t remove_count{};
        const char *file_name{};
        int disabled_macro_index{};  // Macro should not call itself
    };

    std::list<Macro> macro;
    std::list<Stack> stack;

public:
    std::function<const char *(const char *, size_t)> save_string;
    std::function<void(std::string &)> preprocessor;

    CErrorPosition error_position;  // Что бы указывать в тексте ошибки места, где был вызван первый макрос
    const char *file_name{};

    void Open(const char *contents, const char *name);
    void Include(const char *contents, const char *name);
    void AddMacro(CString name, const char *body, size_t size, const std::vector<std::string> *args = nullptr);
    bool FindMacro(CString name);
    bool DeleteMacro(CString name);
    void NextToken();
    void SyntaxError();
    void Throw(CString text);

    bool ReadDirective(std::string &out);

private:
    bool Leave();
    void Enter(int remove_count, int macro_index, const char *contents, const char *name);
    void ReadDirectiveAppend(std::string &result, const char *&line_start, unsigned remove_eol);
};

inline CErrorPosition::CErrorPosition(const CLex &lex) {
    if (lex.error_position.file_name != nullptr) {
        *this = lex.error_position;
    } else {
        line = lex.line;
        column = lex.column;
        cursor = lex.cursor;
        file_name = lex.file_name;
    }
}
