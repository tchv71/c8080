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

#include "getnumberasuint64.h"
#include "cthrow.h"

uint64_t GetNumberAsUint64(CNodePtr node) {
    assert(node != nullptr);

    if (node->type != CNT_NUMBER)
        C_INTERNAL_ERROR(node, "incorrect node type");

    switch (node->ctype.GetAsmType()) {
        case CBT_CHAR:
        case CBT_SHORT:
        case CBT_LONG:
        case CBT_LONG_LONG:
            return uint64_t(node->number.i);
        case CBT_UNSIGNED_CHAR:
        case CBT_UNSIGNED_SHORT:
        case CBT_UNSIGNED_LONG:
        case CBT_UNSIGNED_LONG_LONG:
            return node->number.u;
        case CBT_FLOAT:
            return uint64_t(node->number.f);
        case CBT_DOUBLE:
            return uint64_t(node->number.d);
        case CBT_LONG_DOUBLE:
            return uint64_t(node->number.ld);
    }

    C_INTERNAL_ERROR(node, "incorrect data type");
    return 0;
}
