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
#include "tools/cthrow.h"

// Get size of base type

uint8_t SizeOf(CBaseType type, CConstErrorPosition &e) {
    switch (type) {
        case CBT_VOID:
            return 0;
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
        case CBT_SIGNED_CHAR:
            return C_SIZEOF_CHAR;
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
            return C_SIZEOF_SHORT;
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
        case CBT_STRUCT:  // Must use CType::SizeOf()
            CThrow(e, "Internal error, sizeof(struct) in " + std::string(__PRETTY_FUNCTION__));
        case CBT_FUNCTION:
            CThrow(e, "C forbids applying 'sizeof' to an expression of function type");  // gcc
        case CBT_VA_LIST:
            CThrow(e, "sizeof(__builtin_va_list) not implemented");
    }
    CThrow(e, "Internal error, sizeof(unknown " + std::to_string(type) + ") in " + std::string(__PRETTY_FUNCTION__));
    return 1;  // no return
}

// Check for integer base type

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
        case CBT_STRUCT:
        case CBT_FUNCTION:
        case CBT_VOID:
        case CBT_FLOAT:
        case CBT_DOUBLE:
        case CBT_LONG_DOUBLE:
        case CBT_VA_LIST:
            return false;
    }
    return false;
}

// Check for unsigned base type

bool IsUnsigned(CBaseType type) {
    switch (type) {
        case CBT_UNSIGNED_CHAR:
        case CBT_UNSIGNED_SHORT:
        case CBT_UNSIGNED_INT:
        case CBT_UNSIGNED_LONG:
        case CBT_UNSIGNED_LONG_LONG:
            return true;
        case CBT_STRUCT:
        case CBT_FUNCTION:
        case CBT_VOID:
        case CBT_CHAR:
        case CBT_SIGNED_CHAR:
        case CBT_SHORT:
        case CBT_INT:
        case CBT_LONG:
        case CBT_LONG_LONG:
        case CBT_FLOAT:
        case CBT_DOUBLE:
        case CBT_LONG_DOUBLE:
        case CBT_VA_LIST:
            return false;
    }
    return false;
}

// Base type of the result after an operation on values ​​of base types
// Example: char + unsigned char => unsigned int

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
        case CBT_STRUCT:
        case CBT_FUNCTION:
        case CBT_VOID:
        case CBT_VA_LIST:
            return CBT_VOID;
    }
    return CBT_VOID;  // Error
}
