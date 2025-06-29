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

#include "Compiler.h"

void Compiler8080::BuildCall(CNodePtr &node, AsmRegister reg) {
    if (reg == REG_PREPARE) {
        node->bi.SetMain(U_ALL, out.CALL_METRICS);
        return;
    }

    if (node->type == CNT_FUNCTION_CALL) {
        assert(node->variable != nullptr);
        MakeCallTreeBegin(node->variable);
    }

    for (CNodePtr *i = &node->c; *i != nullptr; i = &((*i)->next_node))
        CompileCommand(*i);

    std::vector<CNodePtr> args;
    for (CNodePtr *i = &node->a; *i != nullptr; i = &((*i)->next_node))
        args.push_back(*i);

    size_t used_stack_size = 0;

    for (size_t j = args.size(); j != 0; j--) {
        CNodePtr &i = args[j - 1];
        Build(i);
        switch (i->ctype.GetAsmType()) {
            case CBT_CHAR:
            case CBT_UNSIGNED_CHAR:
                assert(reg == R8_A || reg == R8_D);
                if (!i->bi.alt.able) {
                    Build(i, R8_A);
                    out.dec_reg(R16_SP);  // TODO: Remove
                    out.push_af();
                    out.inc_reg(R16_SP);  // TODO: Remove
                } else {
                    Build(i, R8_D);
                    out.push_de();
                }
                used_stack_size += 2;
                break;
            case CBT_SHORT:
            case CBT_UNSIGNED_SHORT:
                if (!i->bi.alt.able) {
                    Build(i, R16_HL);
                    out.push_reg(R16_HL);
                } else {
                    Build(i, R16_DE);
                    out.push_reg(R16_DE);
                }
                used_stack_size += 2;
                break;
            case CBT_LONG:
            case CBT_UNSIGNED_LONG:
                Build(i, R32_DEHL);
                out.push_de_hl();
                used_stack_size += 4;
                break;
            default:
                C_ERROR_UNSUPPORTED_ASM_TYPE(i->ctype.GetAsmType(), i);
        }
    }

    if (node->type == CNT_FUNCTION_CALL) {
        if (node->variable->address_attribute.exists)
            out.call(node->variable->address_attribute.value);
        else
            out.call(node->variable->output_name);

        MakeCallTreeEnd();
    } else {
        Build(node->a);
        Build(node->a, R16_HL);
        InternalCall(o.call_hl);
    }

    AsmRegister result_reg = GetResultReg(node->ctype, false, reg == REG_NONE, node);
    OutAddSpN(result_reg, used_stack_size);
}

void Compiler8080::InternalCall(CVariablePtr &fn) {
    MakeCallTreeBegin(fn);

    if (fn->address_attribute.exists)
        out.call(fn->address_attribute.value);
    else
        out.call(fn->output_name);

    MakeCallTreeEnd();
}

static void AddCalledBy(CVariableCompiler &self, CVariablePtr &function, bool call) {
    for (auto &i : self.called_by) {  // TODO: Optimize, change to map
        if (i.function.lock() == function) {
            if (call)
                i.call = true;
            return;
        }
    }
    self.called_by.push_back(CVariableCompiler::CalledBy{function, call});
    function->c.call_count++;
}

void Compiler8080::MakeCallTreeBegin(CVariablePtr &fn) {
    assert(fn->type.IsFunction());

    if (out.measure)
        return;

    for (auto &i : call_in_call)
        AddCalledBy(fn->c, i, false);
    AddCalledBy(fn->c, current_function, true);

    call_in_call.push_back(fn);

    AddToCompileQueue(fn);
}

void Compiler8080::MakeCallTreeEnd() {
    if (out.measure)
        return;

    call_in_call.pop_back();
}

void Compiler8080::AddToCompileQueue(CVariablePtr &fn) {
    if (fn->c.use_counter == 0) {
        fn->c.use_counter++;

        if (!fn->only_extern)
            out.compile_queue.push_back(fn);
    }
}
