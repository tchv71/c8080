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
#include "../c/tools/numberiszero.h"
#include "../c/tools/numberisone.h"
#include "../c/tools/numberisnonzero.h"

bool PrepareRemoveUselessOperations(Prepare &p, CNodePtr &node) {
    if (node->type == CNT_OPERATOR) {
        switch (node->operator_code) {
            case COP_SHL:
                if (NumberIsZero(node->b))  // Replace X << 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SHR:
                if (NumberIsZero(node->b))  // Replace X >> 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_OR:
                if (NumberIsZero(node->b))  // Replace X | 0 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NumberIsZero(node->a))  // Replace 0 | X with X
                    return DeleteNodeSaveType(node, 'b');
                // TODO: 0xFFFF
                return false;
            case COP_XOR:
                if (NumberIsZero(node->b))  // Replace X ^ 0 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NumberIsZero(node->a))  // Replace 0 ^ X with X
                    return DeleteNodeSaveType(node, 'b');
                return false;
            case COP_ADD:
                if (NumberIsZero(node->b))  // Replace X + 0 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NumberIsZero(node->a))  // Replace 0 + X with X
                    return DeleteNodeSaveType(node, 'b');
                return false;
            case COP_SUB:
                if (NumberIsZero(node->b))  // Replace X - 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_MUL:
                if (NumberIsZero(node->a)) {
                    std::swap(node->a, node->b);
                    node->operator_code = COP_COMMA;
                    return true;
                }
                if (NumberIsZero(node->b)) {
                    node->operator_code = COP_COMMA;
                    return true;
                }
                if (NumberIsOne(node->b))  // Replace X * 1 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NumberIsOne(node->a))  // Replace 1 * X with X
                    return DeleteNodeSaveType(node, 'b');
                // TODO: 0
                return false;
            case COP_DIV:
                if (NumberIsOne(node->b))  // Replace X / 1 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_LAND:
                if (NumberIsNonZero(node->b))  // Replace X && true with X
                    return DeleteNodeSaveType(node, 'a');
                if (NumberIsNonZero(node->a))  // Replace true && X with X
                    return DeleteNodeSaveType(node, 'b');
                // TODO: false
                return false;
            case COP_LOR:
                if (NumberIsZero(node->b))  // Replace X || 0 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NumberIsZero(node->a))  // Replace 0 || X with X
                    return DeleteNodeSaveType(node, 'b');
                // TODO: true
                return false;
            case COP_SET_ADD:
                if (NumberIsZero(node->b))  // Replace X += 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_SUB:
                if (NumberIsZero(node->b))  // Replace X -= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_MUL:              // TODO: Not work
                if (NumberIsOne(node->b))  // Replace X *= 1 with X
                    return DeleteNodeSaveType(node, 'a');
                // TODO: 0
                return false;
            case COP_SET_DIV:              // TODO: Not work
                if (NumberIsOne(node->b))  // Replace X /= 1 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_SHR:
                if (NumberIsZero(node->b))  // Replace X >>= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_SHL:
                if (NumberIsZero(node->b))  // Replace X <<= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_OR:
                if (NumberIsZero(node->b))  // Replace X |= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
                // TODO: 0xFFFF
            case COP_SET_XOR:
                if (NumberIsZero(node->b))  // Replace X ^= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
                // TODO: COP_CMP_L
                // TODO: COP_CMP_G
                // TODO: COP_CMP_LE
                // TODO: COP_CMP_GE
                // TODO: COP_CMP_E
                // TODO: COP_CMP_NE
                // TODO: COP_MOD
                // TODO: COP_AND
                // TODO: COP_SET_AND
        }
    }
    return false;
}
