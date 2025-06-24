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

#include "cnode.h"
#include <stdexcept>

bool DeleteNode(CNodePtr &node, char c) {
    assert(node != nullptr);
    CNodePtr saved_child;
    switch (c) {
        case 'a':
            saved_child = node->a;
            break;
        case 'b':
            saved_child = node->b;
            break;
        case 'c':
            saved_child = node->c;
            break;
        default:
            throw std::runtime_error("Internal error, invalid argument " + std::to_string(int(c)) + " in " +
                                     std::string(__PRETTY_FUNCTION__));
    }
    if (saved_child->next_node != nullptr)
        throw std::runtime_error("Internal error, next_node != nullptr in " + std::string(__PRETTY_FUNCTION__));

    CNodePtr saved_next_node = node->next_node;
    *node = *saved_child;
    node->next_node = saved_next_node;
    return true;
}

CNodePtr CopyNode(CNodePtr source) {
    if (source == nullptr)
        return nullptr;

    CNodePtr result = std::make_shared<CNode>();
    *result = *source;
    result->a = CopyNode(source->a);
    result->b = CopyNode(source->b);
    result->c = CopyNode(source->c);
    result->d = CopyNode(source->d);
    result->next_node = CopyNode(source->next_node);
    return result;
}

bool CNode::IsJumpNode() {
    switch (type) {
        case CNT_OPERATOR:
            if (operator_code == COP_LAND || operator_code == COP_LOR)
                return true;
            return IsCompareOperator(operator_code);
        case CNT_MONO_OPERATOR:
            return mono_operator_code == MOP_NOT;
    }
    return false;
}
