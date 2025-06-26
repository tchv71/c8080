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

bool PrepareDoubleConvert(Prepare &, CNodePtr &node) {
    if (node->type == CNT_CONVERT) {
        const CBaseType a = node->ctype.GetAsmTypeIgnoreSign();
        const CBaseType b = node->a->ctype.GetAsmTypeIgnoreSign();

        // Remove cast from type to same type
        if (a == b) {
            DeleteNodeSaveType(node, 'a');
            return true;
        }

        // Replace cast "medium size -> large size -> small size" with "medium size -> small size"
        if (node->a->type == CNT_CONVERT) {
            const CBaseType c = node->a->a->ctype.GetAsmTypeIgnoreSign();
            if (IsInteger(a) && IsInteger(b) && IsInteger(c)) {
                if (a <= c && c <= b) {
                    DeleteNode(node->a, 'a');
                    return true;
                }
            }
        }
    }
    return false;
}
