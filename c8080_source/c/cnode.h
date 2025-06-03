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
#include "clinkattribute.h"
#include "cerrorposition.h"
#include "cconststring.h"
#include "coperatorcode.h"
#include "cmonooperatorcode.h"
#include "ctype.h"
#include "cvariable.h"
#include "cstruct.h"

class AssemblerLabel;

enum CNodeType {
    CNT_NUMBER,
    CNT_LOAD_VARIABLE,
    CNT_CONST,
    CNT_RETURN,
    CNT_IF,
    CNT_DO,
    CNT_WHILE,
    CNT_BREAK,
    CNT_CONTINUE,
    CNT_SWITCH,
    CNT_CASE,
    CNT_DEFAULT,
    CNT_LEVEL,
    CNT_VARIABLE,
    CNT_OPERATOR,
    CNT_MONO_OPERATOR,
    CNT_SIZEOF_TYPE,
    CNT_FUNCTION_CALL,
    CNT_FUNCTION_CALL_ADDR,
    CNT_TYPEDEF,
    CNT_CONST_STRING,
    CNT_FOR,
    CNT_CONVERT,
    CNT_PUSH_POP,
    CNT_ASM,
    CNT_LABEL,
    CNT_GOTO,
    CNT_SELF_CHANGE,
    CNT_STRUCT_STRING,

    // Добавляются оптимизатором
    CNT_SAVE_TO_REGISTER,  // Регистр процессора в котором передается последний аргумент __fastcall функции
    CNT_LOAD_FROM_REGISTER,  // Регистр процессора в котором передается последний аргумент __fastcall функции
    CNT_STACK_ADDRESS,  // Адрес относительно первой стековой переменной __stdcall функции
    CNT_ARG_STACK_ADDRESS,  // Адрес относительно первого аргумента __stdcall функции

    CNT_DELETED,
};

const char *ToString(CNodeType type);

struct CNode {
    CNodeType type = CNT_NUMBER;
    CNodePtr a;
    CNodePtr b;
    CNodePtr c;
    CNodePtr d;

    std::weak_ptr<CNode> case_link;  // Используется только у SWITCH и CASE. Cвязанный список всех case внутри switch.
    std::weak_ptr<CNode> default_link;  // Используется только у SWITCH

    CType ctype;
    CStructItem *struct_item{};  // Используется только у MOP_STRUCT_ITEM и MOP_STRUCT_ITEM_POINTER
    COperatorCode operator_code{};
    CMonoOperatorCode mono_operator_code{};
    CConstString *const_string{};
    CNodePtr next_node;
    std::shared_ptr<CVariable> variable;
    bool extern_flag{};
    bool const_prepared{};

    union {
        int64_t i;
        uint64_t u;
        double d;
        float f;
        long double ld;
    } number;

    std::string text;

    CLinkAttribute link_attribute;
    CErrorPosition place;

    // Заполняется оптимизатором
    std::vector<std::shared_ptr<CVariable>>
        used_in_the_expression;  // Содержит ссылки на переменные, которые используются в текстовом выражении

    // Метка CASE или DEFAULT. Заполняется компилятором.
    AssemblerLabel *label{};

    bool IsConstNode() const {
        return type == CNT_CONST || type == CNT_CONST_STRING || type == CNT_NUMBER;
    }
};

void DeleteNode(CNodePtr &node, char c);
CNodePtr CopyNode(CNodePtr a);

#define CNODE(X...) (std::make_shared<CNode>(CNode{X}))
