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

#include "cnodetype.h"

std::string ToString(CNodeType type) {
    switch (type) {
        case CNT_STACK_ADDRESS:
            return "STACK_ADDRESS";
        case CNT_ARG_STACK_ADDRESS:
            return "ARG_STACK_ADDRESS";
        case CNT_NUMBER:
            return "NUMBER";
        case CNT_RETURN:
            return "RETURN";
        case CNT_IF:
            return "IF";
        case CNT_WHILE:
            return "WHILE";
        case CNT_LEVEL:
            return "LEVEL";
        case CNT_DECLARE_VARIABLE:
            return "DECLARE_VARIABLE";
        case CNT_OPERATOR:
            return "OPERATOR";
        case CNT_MONO_OPERATOR:
            return "MONO_OPERATOR";
        case CNT_SIZEOF_TYPE:
            return "SIZEOF_TYPE";
        case CNT_LOAD_VARIABLE:
            return "LOAD_VARIABLE";
        case CNT_FUNCTION_CALL:
            return "FUNCTION_CALL";
        case CNT_FUNCTION_CALL_ADDR:
            return "FUNCTION_CALL_ADDR";
        case CNT_PUSH_POP:
            return "PUSH_POP";
        case CNT_LABEL:
            return "LABEL";
        case CNT_GOTO:
            return "GOTO";
        case CNT_TYPEDEF:
            return "TYPEDEF";
        case CNT_CONST_STRING:
            return "CONST_STRING";
        case CNT_STRUCT_STRING:
            return "STRUCT_STRING";
        case CNT_FOR:
            return "FOR";
        case CNT_CONVERT:
            return "CONVERT";
        case CNT_DO:
            return "DO";
        case CNT_BREAK:
            return "BREAK";
        case CNT_CONTINUE:
            return "CONTINUE";
        case CNT_CONST:
            return "CONST";
        case CNT_CASE:
            return "CASE";
        case CNT_DEFAULT:
            return "DEFAULT";
        case CNT_SWITCH:
            return "SWITCH";
        case CNT_ASM:
            return "ASM";
        case CNT_SAVE_TO_REGISTER:
            return "SAVE_TO_REGISTER";
        case CNT_LOAD_FROM_REGISTER:
            return "LOAD_FROM_REGISTER";
        case CNT_SET:
            return "SET";
    }
    return "CNT_" + std::to_string(int(type));
}
