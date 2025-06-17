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

AsmRegister Compile8080::CompileSetOperation(CNodePtr &node, CompileFlags flags) {
    AsmRegister address_reg = CompileExpression(node->a, 0);
    assert(address_reg == R16_HL);

    // The value  of the variable does not need to be read,
    // since the address can be used in calculations:
    //   ld   a, (var2)
    //   add  (hl)
    // Or read from HL to DE:
    //   ld   e, (hl)
    //   inc  hl
    //   ld   d, (hl)
    //   ld   hl, (main_b)
    //   add  hl, de

    switch (node->ctype.GetAsmType()) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR: {
            uint32_t b_use_regs = CheckExpression(node->b, 0);
            if (b_use_regs & U_HL)
                out.push_hl();  // ex_hl_de() cannot be used because the address must be in HL
            // TODO: Variant ld_de_hl + ld_pde_a
            AsmRegister value_reg = CompileExpression(node->b, 0);
            assert(value_reg == R8_A);
            if (b_use_regs & U_HL)
                out.pop_hl();
            out.ld_phl_a();
            return R8_A;
        }
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT: {
            out.push_hl();  // ex_hl_de() cannot be used because the address must be in HL
            AsmRegister value_reg = CompileExpression(node->b, 0);
            assert(value_reg == R16_HL);
            out.ex_hl_de();
            out.pop_hl();
            out.ld_phl_e();
            out.inc_hl();
            out.ld_phl_d();
            // value in de
            if (flags & CF_NO_RESULT)
                return REG_NONE;
            if (flags & CF_CAN_USE_ALT)
                return R16_DE;
            out.ex_hl_de();
            return R16_HL;
        }
        case CBT_LONG:
        case CBT_UNSIGNED_LONG: {
            out.push_hl();  // ex_hl_de() cannot be used because the address must be in HL
            AsmRegister value_reg = CompileExpression(node->b, 0);
            assert(value_reg == R32_DEHL);
            CallInternal(o.set_32);
            out.stack_correction(-2);
            return R32_DEHL;
        }
        default:
            C_INTERNAL_ERROR(node, "unsupported data type");
            return REG_NONE;
    }
}
