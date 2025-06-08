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

#include "cnodeptr.h"
#include "cattribute.h"
#include "cerrorposition.h"
#include "cconststring.h"
#include "coperatorcode.h"
#include "cmonooperatorcode.h"
#include "ctype.h"
#include "cvariable.h"
#include "cstruct.h"
#include "cnodetype.h"
#include "cnodecompiler.h"

union CNodeNumber {
    int64_t i;
    uint64_t u;
    double d;
    float f;
    long double ld;
};

struct CNode {
    CNodeType type{};
    CNodePtr a;
    CNodePtr b;
    CNodePtr c;
    CNodePtr d;
    CType ctype;
    CNodePtr next_node;

    std::weak_ptr<CNode> case_link;              // Only in SWITCH and CASE. Linked list.
    std::weak_ptr<CNode> default_link;           // Only in SWITCH
    CStructItemPtr struct_item;                  // Only in MOP_STRUCT_ITEM and MOP_STRUCT_ITEM_POINTER
    COperatorCode operator_code{};               // Only in OPERATOR
    CMonoOperatorCode mono_operator_code{};      // Only in MONO_OPERATOR
    CNodeNumber number{};                        // Only in NUMBER
    CConstStringPtr const_string;                // Only in CONST_STRING
    CVariablePtr variable;                       // Only in DECLARE_VARIABLE, LOAD_VARIABLE, FUNCTION_CALL*
    bool extern_flag{};                          // Only in DECLARE_VARIABLE
    CLinkAttribute link_attribute;               // Only in DECLARE_VARIABLE
    CAddressAttribute address_attribute;         // Only in DECLARE_VARIABLE
    std::string text;                            // Only in CONST or ASM
    CErrorPosition e;

    CNodeCompiler compiler;

    bool IsConstNode() const {
        return type == CNT_CONST || type == CNT_CONST_STRING || type == CNT_NUMBER;
    }
};

void DeleteNode(CNodePtr &node, char c);
CNodePtr CopyNode(CNodePtr a);

#define CNODE(...) (std::make_shared<CNode>(CNode{__VA_ARGS__}))
