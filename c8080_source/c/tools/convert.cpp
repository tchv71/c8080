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

#include "convert.h"
#include "numberiszero.h"
#include "../tools/cthrow.h"

// These types convert to each other without errors

static bool AllowedConversions(CConstType type) {
    if (type.IsPointer())
        return false;
    switch (type.base_type) {
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
        case CBT_FLOAT:
        case CBT_DOUBLE:
        case CBT_LONG_DOUBLE:
            return true;
            // TODO: Остальные типы
    }
    return false;
}

CNodePtr Convert(CConstType to_type, CNodePtr from, bool cmm) {
    assert(from != nullptr);

    CType from_type = from->ctype;

    // При получении значения CONST переменной, этот CONST ничего не значит
    // const int a = 2;
    // int b = a;
    // int* const a = 2;
    // int* b = a;
    if (from_type.pointers.empty())
        from_type.flag_const = false;
    else
        from_type.pointers.back().flag_const = false;

    // void* -> any*
    if (from->ctype.IsVoidPointerIgnoreConst() && to_type.pointers.size() != 0 &&
        (!to_type.pointers[0].flag_const || from->ctype.pointers[0].flag_const)) {
        from->ctype = to_type;
        return from;
    }

    // 0 -> any*
    if (NumberIsZero(from) && to_type.pointers.size() != 0) {
        from->number.u = 0;
        from->ctype = to_type;
        return from;
    }

    // Forgot size of last array level
    if ((from_type.pointers.size() > 0) && (to_type.pointers.size() == from_type.pointers.size()) &&
        (to_type.pointers.back().count == 0) && (from_type.pointers.back().count != 0)) {
        bool need_drop = true;
        for (size_t i = 0u; i < (to_type.pointers.size() - 1u); i++) {
            if (from_type.pointers[i].count != to_type.pointers[i].count) {
                need_drop = false;
                break;
            }
        }
        if (need_drop) {
            // from = new Node{Node::MONO_OPERATOR, a: from, ctype: from->ctype, mono_operator_code: MOP_ADDR, e:
            // from->e};
            from_type.pointers.back().count = 0;
            // from_type = from->ctype;
        }
    }

    // Get const
    if (to_type.pointers.size() == from_type.pointers.size()) {
        for (size_t i = 0; i < to_type.pointers.size(); i++)
            if (to_type.pointers[i].flag_const)
                from_type.pointers[i].flag_const = true;
        if (to_type.flag_const)
            from_type.flag_const = true;
    }

    // any** -> void*
    if (to_type.IsVoidPointerIgnoreConst() && from_type.pointers.size() != 0) {
        // А что там с const в многоуровневом укзателе?
        from_type.pointers = to_type.pointers;
        from_type.base_type = CBT_VOID;
        from_type.variables_mode = CVM_DEFAULT;
        from_type.function_args.clear();
    }

    // Done
    if (from_type.CompareNoStatic(to_type)) {
        from->ctype = from_type;
        // The conversion occurs without additional nodes // TODO
        return from;
    }

    // Simple types are converts any to any
    if ((AllowedConversions(to_type) && AllowedConversions(from_type)) || cmm)
        return CNODE(CNT_CONVERT, a : from, ctype : to_type, e : from->e);

    CThrow(from, "Can't convert from " + from->ctype.ToString() + " to " + to_type.ToString());
    return from;
}
