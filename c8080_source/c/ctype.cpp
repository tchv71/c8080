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

#include "ctype.h"
#include "cstruct.h"
#include "consts.h"
#include "tools/cthrow.h"
#include <stdexcept>

bool CType::CompareNoStatic(const CType &b) const {
    if (base_type != b.base_type || flag_const != b.flag_const || flag_volatile != b.flag_volatile ||
        GetVariableMode() != b.GetVariableMode() || pointers != b.pointers)
        return false;

    switch (base_type) {
        case CBT_FUNCTION:
            if (function_args.size() != b.function_args.size() || many_function_args != b.many_function_args)
                return false;
            for (size_t i = 0; i < function_args.size(); i++)
                if (function_args[i].type != b.function_args[i].type)
                    return false;
            return true;
        case CBT_STRUCT:
            return struct_object == b.struct_object;
        default:
            return true;
    }
}

bool CType::operator==(const CType &b) const {
    if (!CompareNoStatic(b))
        return false;
    return flag_static == b.flag_static;
}

std::string CType::ToString() const {
    std::string result;
    if (flag_const)
        result += "const ";
    if (flag_volatile)
        result += "volatile ";

    if (variables_mode == CVM_GLOBAL)
        result += "__fastcall ";
    else if (variables_mode == CVM_STACK)
        result += "__stdcall ";

    switch (base_type) {
        case CBT_STRUCT:
            assert(struct_object != nullptr);
            result += (struct_object->is_union ? "union " : "struct ");
            result += struct_object->name;
            break;
        case CBT_FUNCTION:
            assert(!function_args.empty());
            result += (function_args.empty() ? "?" : function_args.front().type.ToString());
            break;
        case CBT_VOID:
            result += "void";
            break;
        case CBT_CHAR:
            result += "char";
            break;
        case CBT_UNSIGNED_CHAR:
            result += "unsigned char";
            break;
        case CBT_SIGNED_CHAR:
            result += "signed char";
            break;
        case CBT_SHORT:
            result += "short";
            break;
        case CBT_UNSIGNED_SHORT:
            result += "unsigned short";
            break;
        case CBT_INT:
            result += "int";
            break;
        case CBT_UNSIGNED_INT:
            result += "unsigned int";
            break;
        case CBT_LONG:
            result += "long";
            break;
        case CBT_UNSIGNED_LONG:
            result += "unsigned long";
            break;
        case CBT_LONG_LONG:
            result += "long long";
            break;
        case CBT_UNSIGNED_LONG_LONG:
            result += "unsigned long long";
            break;
        case CBT_FLOAT:
            result += "float";
            break;
        case CBT_DOUBLE:
            result += "double";
            break;
        case CBT_LONG_DOUBLE:
            result += "long double";
            break;
        case CBT_VA_LIST:
            result += "va_list";
            break;
        default:
            assert(false);
            result += "?";
    }
    if (base_type == CBT_FUNCTION && !pointers.empty()) {
        result += "(";
    }
    for (auto &i : pointers) {
        if (i.count == 0) {
            result += "*";
        } else {
            result += "[";
            result += std::to_string(i.count);
            result += "]";
        }
        if (i.flag_const) {
            result += " const";
        }
        if (i.flag_volatile) {
            result += " volatile";
        }
        if (i.flag_restrict) {
            result += " restict";
        }
    }
    if (base_type == CBT_FUNCTION && !pointers.empty()) {
        result += ")";
    }
    if (base_type == CBT_FUNCTION) {
        result += "(";
        for (size_t i = 1; i < function_args.size(); i++) {
            if (i > 1) {
                result += ", ";
            }
            result += function_args[i].type.ToString();
            if (!function_args[i].name.empty()) {
                result += " ";
                result += function_args[i].name;
            }
        }
        if (many_function_args)
            result += "...";
        result += ")";
    }
    return result;
}

CBaseType CType::GetAsmType() const {
    if (pointers.size() != 0)
        return CBT_UNSIGNED_SHORT;

    switch (base_type) {
        case CBT_SIGNED_CHAR:
            return CBT_CHAR;
        case CBT_INT:
            return CBT_SHORT;
        case CBT_UNSIGNED_INT:
            return CBT_UNSIGNED_SHORT;
        default:
            return base_type;
    }
}

uint64_t CType::SizeOfBase(const CErrorPosition &place) const {
    if (base_type == CBT_STRUCT) {
        if (struct_object == nullptr)
            CThrow(place, "Struct is not declared");
        if (!struct_object->inited)
            CThrow(place, "Struct " + struct_object->name + " is not declared");
        return struct_object->size_bytes;
    }
    return ::SizeOf(base_type, place);
}

uint64_t CType::SizeOf(const CErrorPosition &place) const {
    uint64_t total_size = 1;
    for (size_t i = pointers.size(); i != 0; i--) {
        if (pointers[i - 1].count == 0)
            return total_size * C_SIZEOF_POINTER;  // TODO: overflow
        total_size *= pointers[i - 1].count;       // TODO: overflow
    }
    return total_size * SizeOfBase(place);
}

uint64_t CType::SizeOfElement(const CErrorPosition &place) const {
    if (pointers.empty())
        return 1;  // Multiplier of a regular variable (unsigned i; i++;)
    uint64_t total_size = 1;
    for (size_t i = pointers.size() - 1; i != 0; i--) {
        if (pointers[i - 1].count == 0)
            return total_size * C_SIZEOF_POINTER;  // TODO: overflow
        total_size *= pointers[i - 1].count;       // TODO: overflow
    }
    return total_size * SizeOfBase(place);
}
