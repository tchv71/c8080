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

#include "../cnode.h"
#include <iostream>

static void DumpNumber(CConstNodePtr i) {
    if (i->ctype.IsPointer()) {
        std::cout << i->number.u;
        return;
    }
    switch (i->ctype.base_type) {
        case CBT_CHAR:
        case CBT_SIGNED_CHAR:
        case CBT_SHORT:
        case CBT_INT:
        case CBT_LONG:
        case CBT_LONG_LONG:
            std::cout << i->number.i;
            return;
        case CBT_UNSIGNED_CHAR:
        case CBT_UNSIGNED_SHORT:
        case CBT_UNSIGNED_INT:
        case CBT_UNSIGNED_LONG:
        case CBT_UNSIGNED_LONG_LONG:
            std::cout << i->number.u;
            return;
        case CBT_FLOAT:
            std::cout << i->number.f;
            return;
        case CBT_DOUBLE:
            std::cout << i->number.d;
            return;
        case CBT_LONG_DOUBLE:
            std::cout << i->number.ld;
            return;
        case CBT_STRUCT:
            std::cout << "STRUCT";
            return;
        case CBT_FUNCTION:
            std::cout << "FUNCTION";
            return;
        case CBT_VOID:
            std::cout << "VOID";
            return;
        case CBT_VA_LIST:
            std::cout << "VA_LIST";
            return;
    }
    std::cout << "UNKNOWN";
}

void Dump(CNodePtr i, const std::string &level) {
    while (i != nullptr) {
        std::cout << level;
        std::cout << ToString(i->type);
        if (i->extern_flag)
            std::cout << " EXTERN";
        if (i->type == CNT_OPERATOR)
            std::cout << " " << ToString(i->operator_code);
        if (i->type == CNT_MONO_OPERATOR)
            std::cout << " " << ToString(i->mono_operator_code);
        std::cout << " CTYPE " << i->ctype.ToString();
        if (!i->text.empty())
            std::cout << " TEXT " << i->text;
        if (i->type == CNT_NUMBER) {
            std::cout << " NUMBER ";
            DumpNumber(i);
        }
        if (i->variable != nullptr)
            std::cout << " VARIABLE " << i->variable->type.ToString() << " " << i->variable->name;
        if (i->e.line != 0 || i->e.column != 0 || i->e.file_name != nullptr) {
            std::cout << " e " << i->e.line << ", " << i->e.column;
            if (i->e.file_name != nullptr)
                std::cout << ", " << i->e.file_name;
        }
        std::cout << std::endl;

        // TODO: case_link, default_link, struct_item, const_string, include_attribute, address_attribute

        // Children

        if (i->a && !i->b && !i->c && !i->d) {
            Dump(i->a, level + " ");
        } else {
            std::string next_level = level + "  ";
            if (i->a) {
                std::cout << level << " A:" << std::endl;
                Dump(i->a, next_level);
            }
            if (i->b) {
                std::cout << level << " B:" << std::endl;
                Dump(i->b, next_level);
            }
            if (i->c) {
                std::cout << level << " C:" << std::endl;
                Dump(i->c, next_level);
            }
            if (i->d) {
                std::cout << level << " D:" << std::endl;
                Dump(i->d, next_level);
            }
        }
        i = i->next_node;
    }
}
