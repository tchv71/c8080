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

// i8080 have not "sub hl, de" instruction

bool Prepare8080Sub16ToAdd16(Prepare &, CNodePtr &node) {
    if (node->type == CNT_OPERATOR && node->operator_code == COP_SUB) {
        if (node->b->type == CNT_NUMBER) {
            switch (node->ctype.GetAsmType()) {
                case CBT_SHORT:
                    if (node->b->number.i >= 0 && node->b->number.i <= 3)
                        return false; // Compile: dec hl, dec hl, dec hl
                    node->operator_code = COP_ADD;
                    node->b->number.i = 0 - node->b->number.i;
                    return true;
                case CBT_UNSIGNED_SHORT:
                    if (node->b->number.u <= 3)
                        return false; // Compile: dec hl, dec hl, dec hl
                    node->operator_code = COP_ADD;
                    node->b->number.u = (0u - node->b->number.u) & 0xFFFFu;
                    return true;
            }
        } else if (node->b->type == CNT_CONST) {
            node->operator_code = COP_ADD;
            node->b->text = "(0 - (" + node->b->text + ")) & 0FFFFh";
            return true;
        }
    }
    return false;
}
