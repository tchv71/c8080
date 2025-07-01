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

#include "ctype.h"
#include "cerrorposition.h"
#include "cnodeptr.h"
#include "cattribute.h"
#include "../8080/treeextensions/cvariable8080.h"

struct CVariable {
    CType type;
    bool only_extern{};
    bool is_stack_variable{};
    bool is_function_argument{};  // To select ARG_STACK_ADDRESS or STACK_ADDRESS
    bool is_label{};              // For all_top_variables
    CNodePtr body;                // Function body or expression for initializing a variable
    std::string name;
    std::string output_name;
    std::vector<std::shared_ptr<CVariable>> function_arguments;
    size_t function_stack_frame_size{};
    CErrorPosition e;
    uint64_t stack_offset{};
    size_t label_call_count{};
    CAddressAttribute address_attribute;
    CLinkAttribute link_attribute;
    bool link_attribute_processed{};

    struct CVariable8080 c;
};

typedef std::shared_ptr<CVariable> CVariablePtr;
