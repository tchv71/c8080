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

#include "cbasetype.h"
#include <stdexcept>
#include "consts.h"

uint8_t SizeOf(CBaseType type) {
    switch (type) {
        case CBT_VOID:
            return 0;
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
        case CBT_SIGNED_CHAR:
            return 1;
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
            return 2;
        case CBT_INT:
        case CBT_UNSIGNED_INT:
            return C_SIZEOF_INT;
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
            return C_SIZEOF_LONG;
        case CBT_LONG_LONG:
        case CBT_UNSIGNED_LONG_LONG:
            return C_SIZEOF_LONG_LONG;
        case CBT_FLOAT:
            return C_SIZEOF_FLOAT;
        case CBT_DOUBLE:
            return C_SIZEOF_DOUBLE;
        case CBT_LONG_DOUBLE:
            return C_SIZEOF_LONG_DOUBLE;
        case CBT_STRUCT:
            throw std::runtime_error("Can't sizeof(struct)");
        case CBT_FUNCTION:
            throw std::runtime_error("Can't sizeof(function)");
        case CBT_VA_LIST:
            throw std::runtime_error("Can't sizeof(va_list)");
    }
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " " + std::to_string(type));
}

bool IsInteger(CBaseType type) {
    switch (type) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
        case CBT_SIGNED_CHAR:
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
        case CBT_INT:
        case CBT_UNSIGNED_INT:
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
        case CBT_LONG_LONG:
        case CBT_UNSIGNED_LONG_LONG:
            return true;
        default:
            return false;
    }
}

bool IsUnsigned(CBaseType type) {
    switch (type) {
        case CBT_UNSIGNED_CHAR:
        case CBT_UNSIGNED_SHORT:
        case CBT_UNSIGNED_INT:
        case CBT_UNSIGNED_LONG:
        case CBT_UNSIGNED_LONG_LONG:
            return true;
        default:
            return false;
    }
}

CBaseType CalcResultCBaseType(CBaseType a, CBaseType b) {
    static_assert(CBT_CHAR < CBT_UNSIGNED_CHAR, "");
    static_assert(CBT_UNSIGNED_CHAR < CBT_SIGNED_CHAR, "");
    static_assert(CBT_SIGNED_CHAR < CBT_SHORT, "");
    static_assert(CBT_SHORT < CBT_UNSIGNED_SHORT, "");
    static_assert(CBT_UNSIGNED_SHORT < CBT_INT, "");
    static_assert(CBT_INT < CBT_UNSIGNED_INT, "");
    static_assert(CBT_UNSIGNED_INT < CBT_LONG, "");
    static_assert(CBT_LONG < CBT_UNSIGNED_LONG, "");
    static_assert(CBT_UNSIGNED_LONG < CBT_LONG_LONG, "");
    static_assert(CBT_LONG_LONG < CBT_UNSIGNED_LONG_LONG, "");
    static_assert(CBT_UNSIGNED_LONG_LONG < CBT_FLOAT, "");
    static_assert(CBT_FLOAT < CBT_DOUBLE, "");
    static_assert(CBT_DOUBLE < CBT_LONG_DOUBLE, "");

    switch (std::max(a, b)) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
        case CBT_SIGNED_CHAR:
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
        case CBT_INT:
        case CBT_UNSIGNED_INT:
            return (IsUnsigned(a) || IsUnsigned(b)) ? CBT_UNSIGNED_INT : CBT_INT;
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
            return (IsUnsigned(a) || IsUnsigned(b)) ? CBT_UNSIGNED_LONG : CBT_LONG;
        case CBT_LONG_LONG:
        case CBT_UNSIGNED_LONG_LONG:
            return (IsUnsigned(a) || IsUnsigned(b)) ? CBT_UNSIGNED_LONG_LONG : CBT_LONG_LONG;
        case CBT_FLOAT:
            return CBT_FLOAT;
        case CBT_DOUBLE:
            return CBT_DOUBLE;
        case CBT_LONG_DOUBLE:
            return CBT_LONG_DOUBLE;
        default:
            return CBT_VOID;  // Error
    }
}
