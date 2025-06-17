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

#include "compile_compile.h"

void Compile8080::CompileOperator32(CNodePtr &node) {
    assert(node->type == CNT_OPERATOR);
    assert(node->ctype.Is32BitType());
    assert(node->a != nullptr);
    assert(node->b != nullptr);

    // Compile: OPERATOR(op, x, NUMBER)
    if (node->b->type == CNT_NUMBER)
        if (CompileOperator32Number(node))
            return;

    // Compile: OPERATOR(op, x, y)
    CompileHlDe32(node->a, node->b);
    switch (node->operator_code) {
        case COP_ADD:
            CallInternal(o.add_32);
            break;
        case COP_SUB:
            CallInternal(o.sub_32);
            break;
        case COP_AND:
            CallInternal(o.and_32);
            break;
        case COP_OR:
            CallInternal(o.or_32);
            break;
        case COP_XOR:
            CallInternal(o.xor_32);
            break;
        case COP_MUL:
            CallInternal(node->ctype.IsUnsigned() ? o.mul_u32 : o.mul_i32);
            break;
        case COP_DIV:
            CallInternal(node->ctype.IsUnsigned() ? o.div_u32 : o.div_i32);
            break;
        case COP_MOD:
            CallInternal(node->ctype.IsUnsigned() ? o.mod_u32 : o.mod_i32);
            break;
        case COP_SHL:
            CallInternal(o.shl_32);
            break;
        case COP_SHR:
            CallInternal(node->ctype.IsUnsigned() ? o.shr_u32 : o.shr_i32);
            break;
        default:
            C_INTERNAL_ERROR(node, std::string("unsupported operator ") + ToString(node->operator_code));
    }
    out.stack_correction(-4);
}

void Compile8080::CompileHlDe32(CNodePtr &a, CNodePtr &b) {
    const AsmRegister a_reg = CompileExpression(a);
    assert(a_reg == R32_DEHL);
    out.push_de_hl();
    const AsmRegister b_reg = CompileExpression(b);
    assert(b_reg == R32_DEHL);
}
