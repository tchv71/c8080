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

#include "coperatorcode.h"
#include <stdexcept>

bool IsSetOperator(COperatorCode code) {
    switch (code) {
        case OP_SET:
        case OP_SET_ADD:
        case OP_SET_SUB:
        case OP_SET_MUL:
        case OP_SET_DIV:
        case OP_SET_MOD:
        case OP_SET_SHR:
        case OP_SET_SHL:
        case OP_SET_AND:
        case OP_SET_OR:
        case OP_SET_XOR:
            return true;
        case OP_CMP_L:
        case OP_CMP_G:
        case OP_CMP_LE:
        case OP_CMP_GE:
        case OP_CMP_E:
        case OP_CMP_NE:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_SHR:
        case OP_SHL:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_LAND:
        case OP_LOR:
        case OP_IF:
        case OP_COMMA:
            return false;
    }
    return false;
}

bool IsCompareOperator(COperatorCode code) {
    switch (code) {
        case OP_CMP_E:
        case OP_CMP_NE:
        case OP_CMP_LE:
        case OP_CMP_GE:
        case OP_CMP_L:
        case OP_CMP_G:
            return true;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_SHR:
        case OP_SHL:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_LAND:
        case OP_LOR:
        case OP_SET:
        case OP_SET_ADD:
        case OP_SET_SUB:
        case OP_SET_MUL:
        case OP_SET_DIV:
        case OP_SET_MOD:
        case OP_SET_SHR:
        case OP_SET_SHL:
        case OP_SET_AND:
        case OP_SET_OR:
        case OP_SET_XOR:
        case OP_IF:
        case OP_COMMA:
            return false;
    }
    return false;
}

const char *ToString(COperatorCode code) {
    switch (code) {
        case OP_CMP_L:
            return "<";
        case OP_CMP_G:
            return ">";
        case OP_CMP_LE:
            return "<=";
        case OP_CMP_GE:
            return ">=";
        case OP_CMP_E:
            return "==";
        case OP_CMP_NE:
            return "!=";
        case OP_ADD:
            return "+";
        case OP_SUB:
            return "-";
        case OP_MUL:
            return "*";
        case OP_DIV:
            return "/";
        case OP_MOD:
            return "%";
        case OP_SHR:
            return ">>";
        case OP_SHL:
            return "<<";
        case OP_AND:
            return "&";
        case OP_OR:
            return "%";
        case OP_XOR:
            return "^";
        case OP_LAND:
            return "&&";
        case OP_LOR:
            return "||";
        case OP_SET:
            return "=";
        case OP_SET_ADD:
            return "+=";
        case OP_SET_SUB:
            return "-=";
        case OP_SET_MUL:
            return "*=";
        case OP_SET_DIV:
            return "/=";
        case OP_SET_MOD:
            return "%=";
        case OP_SET_SHR:
            return ">>=";
        case OP_SET_SHL:
            return "<<=";
        case OP_SET_AND:
            return "&=";
        case OP_SET_OR:
            return "|=";
        case OP_SET_XOR:
            return "^=";
        case OP_IF:
            return "?:";
        case OP_COMMA:
            return ",";
    }
    throw std::runtime_error("Internal error " + std::to_string(int(code)) + " in " + __PRETTY_FUNCTION__);
}
