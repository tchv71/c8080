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

#include "index.h"
#include "../tools/loguint64.h"
#include "../c/tools/getnumberasuint64.h"

bool PrepareReplaceDivMulWithShift(Prepare &p, CNodePtr &node) {
    if (node->type == CNT_OPERATOR && node->b->type == CNT_NUMBER && node->b->ctype.IsUnsigned()) {
        switch (node->operator_code) {
            case COP_MUL:
            case COP_DIV:
            case COP_SET_MUL:
            case COP_SET_DIV:
                const uint16_t shift = LogUint64(GetNumberAsUint64(node->b));
                if (shift == UINT8_MAX)
                    return false;
                assert(node->ctype.GetAsmType() == node->a->ctype.GetAsmType());
                assert(node->ctype.GetAsmType() == node->b->ctype.GetAsmType());
                switch (node->operator_code) {
                    case COP_MUL:  // Replace X * Log2(N) with X << N
                        node->operator_code = COP_SHL;
                        node->b->number.u = shift;
                        return true;
                    case COP_DIV:  // Replace X / Log2(N) with X >> N
                        node->operator_code = COP_SHR;
                        node->b->number.u = shift;
                        return true;
                    case COP_SET_MUL:  // Replace X *= Log2(N) with X <<= N
                        node->operator_code = COP_SET_SHL;
                        node->b->number.u = shift;
                        return true;
                    case COP_SET_DIV:  // Replace X /= Log2(N) with X >>= N
                        node->operator_code = COP_SET_SHR;
                        node->b->number.u = shift;
                        return true;
                }
                assert(false);
                return false;
        }
    }
    // TODO: Signed integer
    return false;
}
