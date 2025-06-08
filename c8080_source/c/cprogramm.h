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
#include <list>
#include "cnode.h"
#include "cconststring.h"
#include "cvariable.h"
#include "cvariableptr.h"

class CProgramm {
public:
    CNodePtr first_node;
    std::map<std::string, std::shared_ptr<CConstString>> const_strings;
    std::map<std::string, CStructPtr> global_structs;
    std::map<std::string, CStructPtr> global_unions;
    std::list<std::string> saved_strings;
    std::map<std::string, std::shared_ptr<CVariable>> global_variables;  // Кроме static
    std::vector<std::shared_ptr<CVariable>> all_top_variables;           // В том числе static
    std::map<std::string, int> used_output_names;
    std::shared_ptr<CVariable> static_stack;
    std::map<std::string, int> asm_names;         // Имена найденные в ASM блоках
    std::shared_ptr<CVariable> current_function;  // Убрать.
    bool cmm = false;                             // Язык CMM
    bool error_flag{};

    CProgramm();
    std::shared_ptr<CVariable> FindVariable(const std::string &name);
    std::shared_ptr<CConstString> RegisterConstString(const std::string &text);
    void AddVariable(std::shared_ptr<CVariable> a);
    void Error(const CErrorPosition& e, CString text);
    void Note(const CErrorPosition& e, CString text);
    const char* SaveString(const char *data, size_t size);
};
