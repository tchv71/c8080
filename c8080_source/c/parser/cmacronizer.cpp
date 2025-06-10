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

#include "cmacronizer.h"
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

void CMacroizer::Open(const char *contents, const char *file_name) {
    stack.clear();
    macro.clear();
    in_macro = 0;
    endif_counter = 0;
    error_position = CErrorPosition();

    Open2(contents, file_name);
}

void CMacroizer::Include(const char *contents, const char *file_name) {
    assert(in_macro == 0);
    Enter(nullptr, contents, file_name);
}

void CMacroizer::ThrowSyntaxError() {
    Throw(std::string("syntax error, unexpected '") + std::string(token_data, token_size) + "'");
}

void CMacroizer::SyntaxError() {
    Error(std::string("syntax error, unexpected '") + std::string(token_data, token_size) + "'");
}

void CMacroizer::Throw(CString text) {
    CErrorPosition p(*this);
    CThrow(p, text);
}

void CMacroizer::Error(CString text) {
    CErrorPosition p(*this);
    if (on_error)
        on_error(p, text);
    else
        CThrow(p, text);
}

void CMacroizer::NextToken() {
    for (;;) {
        NextToken2();

        if (token == CT_EOF) {
            if (Leave())
                continue;
            break;  // There are no more files
        }

        if (token == CT_REMARK || token == CT_EOL)
            continue;

        if (token == CT_IDENT) {
            auto mi = macro.find(CString(token_data, token_size));
            if (mi == macro.end() || mi->second->disabled)  // Macro should not call itself
                break;

            Macro &m = *mi->second;
            if (m.args.size() > 0) {
                NextToken();
                if (token_size != 1 || token_data[0] != '(')
                    ThrowSyntaxError();

                for (size_t j = 0; j < m.args.size(); j++) {
                    std::string arg_body;
                    ReadRaw(arg_body, (j + 1) < m.args.size() ? ',' : ')');
                    AddMacro(m.args[j], arg_body.c_str(), arg_body.size());
                }
            }
            m.disabled = true;  // Macro should not call itself
            Enter(&m, m.body, mi->first.c_str());
            continue;
        }

        break;
    }
}

void CMacroizer::Enter(Macro *active_macro, const char *contents, const char *file_name_) {
    Stack *s = Add(stack);
    if (active_macro) {
        if (in_macro == 0) {
            error_position.line = token_line;
            error_position.column = token_column;
            error_position.cursor = token_data;
            error_position.file_name = file_name;
        }
        in_macro++;
    } else {
        if (in_macro != 0)
            Throw("can't include from macro");
    }
    s->column = column;
    s->line = line;
    s->cursor = cursor;
    s->file_name = file_name;
    s->endif_counter = endif_counter;
    s->active_macro = active_macro;
    file_name = file_name_;
    cursor = contents;
    line = 1;
    column = 1;
    endif_counter = 0;
}

bool CMacroizer::Leave() {
    if (endif_counter != 0)
        Error("unterminated #if");  // gcc

    if (in_macro > 0)
        in_macro--;

    if (stack.empty())
        return false;

    Stack &s = stack.back();

    if (s.active_macro) {
        assert(s.active_macro->disabled);
        for (auto i : s.active_macro->args)
            DeleteMacro(i);
        s.active_macro->disabled = false;
    }

    endif_counter = s.endif_counter;
    file_name = s.file_name;
    cursor = s.cursor;
    line = s.line;
    column = s.column;

    stack.pop_back();

    return true;
}

bool CMacroizer::FindDirective(std::string &result) {
    do {
        NextToken2();
        if (token == CT_EOF)
            return false;
    } while (token_data[0] != '#');
    ReadDirective(result);
    return true;
}

void CMacroizer::ReadDirective(std::string &result) {
    for (;;) {
        const char *start = cursor;
        NextToken2();
        if (token == CT_EOF || token == CT_EOL)
            break;
        if (token != CT_REMARK)
            result.append(start, cursor - start);
    }
}

void CMacroizer::ReadRaw(std::string &result, char terminator) {
    for (;;) {
        const char *start = cursor;
        NextToken2();
        if (token == CT_EOF)
            Throw(std::string("expected ‘") + terminator + "’ at end of input");
        if (token_data[0] == '#')  // MACRO(name #endif), asm { #endif }
            Throw("сan't use # here");
        if (token_size == 1 && token_data[0] == terminator)
            return;
        if (token != CT_REMARK)
            result.append(start, cursor - start);
    }
}

void CMacroizer::AddMacro(CString name, const char *body, size_t size, const std::vector<std::string> *args) {
    // TODO: assert(!in_macro);
    std::shared_ptr<Macro> m = std::make_shared<Macro>();
    m->name = name;                     // Копия в Macro
    m->body = save_string(body, size);  // TODO: Не выделять память, а сохранить указатели???
    if (args != nullptr)
        m->args = *args;
    auto &i = macro[m->name];  // В качестве индекса ссылка на Macro
    m->prev = i;
    i = m;
}

bool CMacroizer::FindMacro(CString name) {
    return macro.find(name) != macro.end();
}

bool CMacroizer::DeleteMacro(CString name) {
    // TODO: assert(!in_macro);
    auto i = macro.find(name);
    if (i == macro.end())
        return false;

    if (i->second->prev)
        i->second = i->second->prev;
    else
        macro.erase(i);

    return true;
}
