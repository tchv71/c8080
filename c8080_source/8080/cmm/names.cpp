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

#include "names.h"
#include "../asm/asmcondition.h"

namespace I8080 {

static CVariablePtr RegisterInternalCmmName(CProgramm &p, const char *name, const CType &type, enum CmmName id,
                                            enum AsmCondition c = JC_Z) {
    CVariablePtr v = std::make_shared<CVariable>();
    v->name = name;
    v->output_name = name;
    v->c.internal_cmm_name = id;
    v->c.asm_condition = c;
    v->type = type;
    p.AddVariable(v);
    p.variables[v->name] = v;
    return v;
}

void RegisterInternalCmmNames(CProgramm &p) {
    CType u8_any;
    u8_any.base_type = CBT_FUNCTION;
    u8_any.function_args.push_back(CStructItem{CBT_UNSIGNED_CHAR});
    u8_any.many_function_args = true;

    RegisterInternalCmmName(p, "flag_z", u8_any, CMM_NAME_FLAG, JC_Z);
    RegisterInternalCmmName(p, "flag_nz", u8_any, CMM_NAME_FLAG, JC_NZ);
    RegisterInternalCmmName(p, "flag_c", u8_any, CMM_NAME_FLAG, JC_C);
    RegisterInternalCmmName(p, "flag_nc", u8_any, CMM_NAME_FLAG, JC_NC);
    RegisterInternalCmmName(p, "flag_m", u8_any, CMM_NAME_FLAG, JC_M);
    RegisterInternalCmmName(p, "flag_p", u8_any, CMM_NAME_FLAG, JC_P);
    RegisterInternalCmmName(p, "flag_pe", u8_any, CMM_NAME_FLAG, JC_PE);
    RegisterInternalCmmName(p, "flag_po", u8_any, CMM_NAME_FLAG, JC_PO);

    const CType reg8_type{CBT_UNSIGNED_CHAR};

    RegisterInternalCmmName(p, "a", reg8_type, CMM_NAME_REG)->c.asm_register = R8_A;
    RegisterInternalCmmName(p, "b", reg8_type, CMM_NAME_REG)->c.asm_register = R8_B;
    RegisterInternalCmmName(p, "c", reg8_type, CMM_NAME_REG)->c.asm_register = R8_C;
    RegisterInternalCmmName(p, "d", reg8_type, CMM_NAME_REG)->c.asm_register = R8_D;
    RegisterInternalCmmName(p, "e", reg8_type, CMM_NAME_REG)->c.asm_register = R8_E;
    RegisterInternalCmmName(p, "h", reg8_type, CMM_NAME_REG)->c.asm_register = R8_H;
    RegisterInternalCmmName(p, "l", reg8_type, CMM_NAME_REG)->c.asm_register = R8_L;

    const CType reg16_type{CBT_UNSIGNED_SHORT};

    RegisterInternalCmmName(p, "bc", reg16_type, CMM_NAME_REG)->c.asm_register = R16_BC;
    RegisterInternalCmmName(p, "de", reg16_type, CMM_NAME_REG)->c.asm_register = R16_DE;
    RegisterInternalCmmName(p, "hl", reg16_type, CMM_NAME_REG)->c.asm_register = R16_HL;
    RegisterInternalCmmName(p, "sp", reg16_type, CMM_NAME_REG)->c.asm_register = R16_SP;

    CType v;
    v.base_type = CBT_FUNCTION;
    v.function_args.push_back(CStructItem{CBT_VOID});

    RegisterInternalCmmName(p, "set_flag_c", v, CMM_NAME_SET_FLAG_C);

    CType v_u8 = v;
    v_u8.function_args.push_back(CStructItem{CBT_UNSIGNED_CHAR});

    RegisterInternalCmmName(p, "invert", v_u8, CMM_NAME_INVERT);

    CType v_u8_u8 = v_u8;
    v_u8_u8.function_args.push_back(CStructItem{CBT_UNSIGNED_CHAR});

    RegisterInternalCmmName(p, "compare", v_u8_u8, CMM_NAME_COMPARE);
    RegisterInternalCmmName(p, "cyclic_rotate_left", v_u8_u8, CMM_NAME_CYCLIC_ROTATE_LEFT);
    RegisterInternalCmmName(p, "cyclic_rotate_right", v_u8_u8, CMM_NAME_CYCLIC_ROTATE_RIGHT);
    RegisterInternalCmmName(p, "carry_rotate_left", v_u8_u8, CMM_NAME_CARRY_ROTATE_LEFT);
    RegisterInternalCmmName(p, "carry_rotate_right", v_u8_u8, CMM_NAME_CARRY_ROTATE_RIGHT);
    RegisterInternalCmmName(p, "carry_add", v_u8_u8, CMM_NAME_CARRY_ADD);
    RegisterInternalCmmName(p, "carry_sub", v_u8_u8, CMM_NAME_CARRY_SUB);
    RegisterInternalCmmName(p, "disable_interrupts", v, CMM_NAME_DISABLE_INTERRUPTS);
    RegisterInternalCmmName(p, "enable_interrupts", v, CMM_NAME_ENABLE_INTERRUPTS);
    RegisterInternalCmmName(p, "nop", v, CMM_NAME_NOP);
    RegisterInternalCmmName(p, "daa", v, CMM_NAME_DAA);

    CType v_u16_any = v;
    v_u16_any.function_args.push_back(CStructItem{CBT_UNSIGNED_SHORT});
    v_u16_any.many_function_args = true;

    RegisterInternalCmmName(p, "push", v_u16_any, CMM_NAME_PUSH);
    RegisterInternalCmmName(p, "pop", v_u16_any, CMM_NAME_POP);

    CType u8_u8;
    u8_u8.base_type = CBT_FUNCTION;
    u8_u8.function_args.push_back(CStructItem{CBT_UNSIGNED_CHAR});
    u8_u8.function_args.push_back(CStructItem{CBT_UNSIGNED_CHAR});

    RegisterInternalCmmName(p, "in", u8_u8, CMM_NAME_IN);
    RegisterInternalCmmName(p, "out", v_u8_u8, CMM_NAME_OUT);

    CType v_u16_u16 = v;
    v_u16_u16.function_args.push_back(CStructItem{CBT_UNSIGNED_SHORT});
    v_u16_u16.function_args.push_back(CStructItem{CBT_UNSIGNED_SHORT});

    RegisterInternalCmmName(p, "swap", v_u16_u16, CMM_NAME_SWAP);
}

}  // namespace I8080
