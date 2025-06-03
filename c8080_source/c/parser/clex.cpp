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

#include "clex.h"
#include <string>
#include <limits.h>
#include <list>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include "../../tools/listext.h"
#include "../tools/cthrow.h"

// TODO: a\EOLb это ab
// TODO: Может ли внутри макроса быть директива препроцессора? Это же все сломает
// TODO: Работа # и ## в макросах

void CLex::Open(const char *contents, const char *name) {
    stack.clear();
    macro.clear();
    error_position.file_name = nullptr;
    line = 1;
    column = 1;
    file_name = name;
    cursor = contents;
    token_data = nullptr;
}

void CLex::Include(const char *contents, const char *name) {
    Enter(0, -1, contents, name);
}

void CLex::SyntaxError() {
    Throw(std::string("syntax error, unexpected '") + std::string(token_data, token_size) + "'");
}

void CLex::Throw(CString text) {
    CErrorPosition p(*this);
    CThrow(p, text);
}

void CLex::NextToken() {
    for (;;) {
        SkipSpaces();

        if (cursor[0] == '#' && preprocessor) {
            std::string directive;
            ReadDirective(directive);
            preprocessor(directive);
            continue;
        }

        NextToken2();

        if (token == CT_EOF) {
            if (Leave())
                continue;
            return;
        }

        if (token == CT_REMARK)
            continue;

        if (token == CT_WORD) {
            size_t macro_index = 0u;
            for (Macro &m : macro) {
                if (!m.disabled && token_size == m.name_size && 0 == memcmp(m.name, token_data, token_size))
                    break;
                macro_index++;
            }
            if (macro_index >= macro.size())
                break;

            Macro &m = Get(macro, macro_index);
            if (m.args.size() > 0) {
                if (cursor[0] != '(')
                    SyntaxError();

                const char *s = cursor;

                cursor++;

                for (size_t j = 0; j < m.args.size(); j++) {
                    const char *end = strchr(cursor, (j + 1) < m.args.size() ? ',' : ')');
                    if (end == nullptr)
                        SyntaxError();
                    AddMacro(m.args[j], cursor, end - cursor);
                    cursor = (char *)end + 1;
                }

                UpdateLineColumn(s);
            }
            m.disabled = true;  // Макрос не должен вызывать сам себя
            Enter(m.args.size(), macro_index, m.body, m.name);
            continue;
        }

        break;
    }
}

void CLex::Enter(int remove_count, int macro_index, const char *contents, const char *name) {
    Stack *m = Add(stack);
    if (macro_index != -1) {
        error_position.line = token_line;
        error_position.column = token_column;
        error_position.cursor = token_data;
        error_position.file_name = file_name;
    }
    m->column = column;
    m->line = line;
    m->cursor = cursor;
    m->file_name = file_name;
    m->disabled_macro_index = macro_index;
    m->remove_count = remove_count;
    file_name = name;
    cursor = contents;
    line = 1;
    column = 1;
    token_data = nullptr;  // Self test
}

bool CLex::Leave() {
    if (stack.empty())
        return false;

    Stack &m = stack.back();
    file_name = m.file_name;

    for (size_t i = 0; i < m.remove_count; i++)
        macro.pop_back();
    if (m.disabled_macro_index != -1) {
        Get(macro, m.disabled_macro_index).disabled = false;
        error_position.file_name = nullptr;
    }

    cursor = m.cursor;
    line = m.line;
    column = m.column;

    token_data = nullptr;  // Self test

    stack.pop_back();
    return true;
}

bool CLex::ReadDirective(std::string &result) {
    const char *inital_cursor = cursor;

    const char *next = strchr(cursor, '#');  // TODO: # может быть в комментарии!!!
    if (next == nullptr)
        return false;
    cursor = next + 1;

    // TODO: Нужно отсеять комментарии
    const char *line_begin = cursor;
    char current_char{};
    for (;;) {
        const char prev_char = current_char;
        current_char = *cursor;
        if (current_char == 0) {  // End of file
            ReadDirectiveAppend(result, line_begin, 0);
            break;
        }
        cursor++;
        if (current_char == '\n') {
            if (prev_char == '\\') {
                ReadDirectiveAppend(result, line_begin, 2);
                continue;
            }
            ReadDirectiveAppend(result, line_begin, 1);
            break;
        }
    }

    UpdateLineColumn(inital_cursor);
    return true;
}

void CLex::ReadDirectiveAppend(std::string &result, const char *&line_start, unsigned remove_eol) {
    size_t line_size = cursor - line_start - remove_eol;
    if (remove_eol != 0 && line_size > 0 && line_start[line_size - 1u] == '\r')  // Windows style EOL
        line_size--;
    result.append(line_start, line_size);
    line_start = cursor;
}

void CLex::AddMacro(CString name, const char *body, size_t size, const std::vector<std::string> *args) {
    Macro *m = Add(macro);
    m->name = save_string(name.c_str(), name.size());
    m->name_size = name.size();
    m->body = save_string(body, size);  // TODO: Не выделять память, а сохранить указатели???
    if (args != nullptr)
        m->args = *args;
}

bool CLex::DeleteMacro(CString name) {
    for (std::list<Macro>::iterator m = macro.begin(), m_end = macro.end(); m != m_end; m++) {
        if (m->name == name) {
            macro.erase(m);
            return true;
        }
    }
    return false;
}

bool CLex::FindMacro(CString name) {
    for (std::list<Macro>::iterator m = macro.begin(), m_end = macro.end(); m != m_end; m++)
        if (m->name == name)
            return true;
    return false;
}
