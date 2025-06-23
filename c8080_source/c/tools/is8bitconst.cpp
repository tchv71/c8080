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

#include "is8bitconst.h"
#include "../../c/tools/cthrow.h"

unsigned Is8BitConst(CConstNodePtr node) {
    assert(node != nullptr);
    if (node->type == CNT_NUMBER) {
        switch (node->ctype.GetAsmType()) {
            case CBT_CHAR:
            case CBT_SIGNED_CHAR:
            case CBT_SHORT:
            case CBT_INT:
            case CBT_LONG:
            case CBT_LONG_LONG:
                if (node->number.i < INT8_MIN)
                    return 0;
                if (node->number.i < 0)
                    return IS8BITCONST_SIGNED;
                if (node->number.i <= INT8_MAX)
                    return IS8BITCONST_SIGNED | IS8BITCONST_UNSIGNED;
                if (node->number.i <= UINT8_MAX)
                    return IS8BITCONST_UNSIGNED;
                return 0;
            case CBT_UNSIGNED_CHAR:
            case CBT_UNSIGNED_SHORT:
            case CBT_UNSIGNED_INT:
            case CBT_UNSIGNED_LONG:
            case CBT_UNSIGNED_LONG_LONG:
                if (node->number.u <= unsigned(INT8_MIN))
                    return IS8BITCONST_SIGNED | IS8BITCONST_UNSIGNED;
                if (node->number.u <= UINT8_MAX)
                    return IS8BITCONST_UNSIGNED;
                return 0;
        }
    }
    return false;
}
