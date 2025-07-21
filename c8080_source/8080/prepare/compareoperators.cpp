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

namespace I8080 {

static bool ReplaceOpUnsigned(CNodePtr &node, COperatorCode op, uint64_t max) {
    if (node->a->number.u == max)
        return false;
    node->a->number.u++;
    std::swap(node->a, node->b);
    node->operator_code = op;
    return true;
}

static bool ReplaceOpSigned(CNodePtr &node, COperatorCode op, int64_t max) {
    if (node->a->number.i == max)
        return false;
    node->a->number.i++;
    std::swap(node->a, node->b);
    node->operator_code = op;
    return true;
}

static bool ReplaceOp(CNodePtr &node, COperatorCode op) {
    switch (node->ctype.GetAsmType()) {
        case CBT_UNSIGNED_CHAR:
            return ReplaceOpUnsigned(node, op, UINT8_MAX);
        case CBT_CHAR:
            return ReplaceOpSigned(node, op, INT8_MAX);
        case CBT_UNSIGNED_SHORT:
            return ReplaceOpUnsigned(node, op, UINT16_MAX);
        case CBT_SHORT:
            return ReplaceOpSigned(node, op, INT16_MAX);
        case CBT_UNSIGNED_LONG:
            return ReplaceOpUnsigned(node, op, UINT32_MAX);
        case CBT_LONG:
            return ReplaceOpSigned(node, op, INT32_MAX);
        case CBT_UNSIGNED_LONG_LONG:
            return ReplaceOpUnsigned(node, op, UINT64_MAX);
        case CBT_LONG_LONG:
            return ReplaceOpSigned(node, op, INT64_MAX);
    }
    return false;
}

bool PrepareCompareOperators(Prepare &, CNodePtr &node) {
    if (node->type != CNT_OPERATOR)
        return false;

    // Replace "X <= Y" with "Y >= X". Not supported by 8080.
    if (node->operator_code == COP_CMP_LE) {
        swap(node->a, node->b);
        node->operator_code = COP_CMP_GE;
        return true;
    }

    // Replace "X > Y" with "Y < X". Not supported by 8080.
    if (node->operator_code == COP_CMP_G) {
        swap(node->a, node->b);
        node->operator_code = COP_CMP_L;
        return true;
    }

    // Replace "CONST < X" with "X >= CONST + 1". For fast "cmp const"
    if (node->operator_code == COP_CMP_GE && node->a->type == CNT_NUMBER)
        if (ReplaceOp(node, COP_CMP_L))
            return true;

    // Replace "CONST < X" with "X >= CONST + 1". For fast "cmp const"
    if (node->operator_code == COP_CMP_L && node->a->type == CNT_NUMBER)
        if (ReplaceOp(node, COP_CMP_GE))
            return true;

    return false;
}

}  // namespace I8080
