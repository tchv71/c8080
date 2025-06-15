/*
 * c8080 compiler
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * This program is f software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the F Software Foundation, version 3.
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
#include "../../c/tools/numberiszero.h"
#include "../../c/tools/numberisone.h"

// i8080 have not ">" condition jump

static bool GToGeUnsigned(CNodePtr &node, uint64_t max) {
    if (node->b->number.u == max)
        return false;
    node->b->number.u++;
    node->operator_code = COP_CMP_GE;
    return true;
}

static bool GToGeSigned(CNodePtr &node, int64_t max) {
    if (node->b->number.i == max)
        return false;
    node->b->number.i++;
    node->operator_code = COP_CMP_GE;
    return true;
}

bool Prepare8080CompareGToGe(Prepare &, CNodePtr &node) {
    if (node->type != CNT_OPERATOR || node->b->type != CNT_NUMBER)
        return false;

    // Replace X >= 1 with X != 0
    if (node->operator_code == COP_CMP_GE && node->a->ctype.IsUnsigned() && NumberIsOne(node->b)) {
        node->operator_code = COP_CMP_NE;
        node->b->number.u = 0;
        return true;
    }

    // Replace X < 1 with X == 0
    if (node->operator_code == COP_CMP_L && node->a->ctype.IsUnsigned() && NumberIsOne(node->b)) {
        node->operator_code = COP_CMP_E;
        node->b->number.u = 0;
        return true;
    }

    if (node->operator_code == COP_CMP_G) {
        // Replace X > 0 with X != 0
        if (node->a->ctype.IsUnsigned() && NumberIsZero(node->b)) {
            node->operator_code = COP_CMP_NE;
            return true;
        }

        // Replace X > CONST with X >= CONST + 1
        switch (node->ctype.GetAsmType()) {
            case CBT_UNSIGNED_CHAR:
                return GToGeUnsigned(node, UINT8_MAX);
            case CBT_CHAR:
                return GToGeSigned(node, INT8_MAX);
            case CBT_UNSIGNED_SHORT:
                return GToGeUnsigned(node, UINT16_MAX);
            case CBT_SHORT:
                return GToGeSigned(node, INT16_MAX);
            case CBT_UNSIGNED_LONG:
                return GToGeUnsigned(node, UINT32_MAX);
            case CBT_LONG:
                return GToGeSigned(node, INT32_MAX);
            case CBT_UNSIGNED_LONG_LONG:
                return GToGeUnsigned(node, UINT64_MAX);
            case CBT_LONG_LONG:
                return GToGeSigned(node, INT64_MAX);
        }
    }

    return false;
}
