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

#include "staticstack.h"
#include "../c/cnodelist.h"
#include "../c/tools/makeoperator.h"
#include "../c/tools/cthrow.h"

// Calculate stack position at compile time
// Replace stack variables with global variables

static bool LastAgumentInCpuRegister(CVariable &f) {
    if (f.type.GetVariableMode() == CVM_GLOBAL && f.type.function_args.size() >= 2u) {  // Min 1 argument
        switch (f.type.function_args.back().type.GetAsmType()) {
            case CBT_CHAR:
            case CBT_UNSIGNED_CHAR:  // 8 bit
            case CBT_SHORT:
            case CBT_UNSIGNED_SHORT:  // 16 bit
            case CBT_LONG:
            case CBT_UNSIGNED_LONG:  // 32 bit
                return true;
        }
    }
    return false;
}

static void PrepareFunctionStaticStack(Prepare &p, CVariable &f) {
    if (f.type.GetVariableMode() != CVM_GLOBAL)
        return;  // Standard stack is used

    if (f.c.static_stack != nullptr)
        return;  // Already executed

    // Allocate static stack
    if (p.programm.c.static_stack == nullptr) {
        auto v = std::make_shared<CVariable>();
        v->output_name = "__static_stack";
        v->name = "?__static_stack";
        v->c.use_counter++;  // Always used
        v->type.base_type = CBT_UNSIGNED_CHAR;
        v->type.pointers.push_back(CPointer{1});  // .count will be filled in the last step
        p.programm.AddVariable(v);
        p.programm.c.static_stack = v;
    }

    // Allocate function static stack
    auto s = std::make_shared<CVariable>();
    s->name = "?__s_" + f.name;
    s->output_name = "__s_" + f.name;
    s->c.use_counter++;  // Always used
    s->type.base_type = CBT_UNSIGNED_CHAR;
    s->type.pointers.push_back(CPointer{0});
    p.programm.AddVariable(s);
    f.c.static_stack = s;

    // Allocate space for function arguments in the static stack
    for (size_t i = 0; i < f.function_arguments.size(); i++) {
        auto &arg = f.function_arguments[i];
        assert(arg != nullptr && arg->is_stack_variable && arg->is_function_argument && !arg->only_extern);
        arg->output_name = "__a_" + std::to_string(i + 1u) + "_" + f.output_name;
        arg->c.use_counter++;            // Always used
        arg->is_stack_variable = false;  // Now it is a global variable / equ
        arg->c.equ_enabled = true;       // TODO: Replace to OPERATOR.ADD()
        arg->c.equ_text = s->output_name + " + " + std::to_string(f.function_stack_frame_size);
        p.programm.AddVariable(arg);

        f.function_stack_frame_size += arg->type.SizeOf(arg->e);
    }
}

bool PrepareStaticArgumentsCall(Prepare &p, CNodePtr &node) {
    if (!p.programm.cmm && node->type == CNT_FUNCTION_CALL && node->a != nullptr &&
        node->variable->type.GetVariableMode() == CVM_GLOBAL) {
        PrepareFunctionStaticStack(p, *node->variable);

        std::vector<CStructItem> &arg_list = node->variable->type.function_args;
        std::vector<CVariablePtr> &arg_variables = node->variable->function_arguments;

        CNodePtr arg = node->a;
        node->a = nullptr;

        CNodeList body;
        for (size_t i = 1; i < arg_list.size(); i++) {
            // Not enough arguments
            if (arg == nullptr)
                C_THROW_TYPE_NOT_SUPPORTED_INTERNAL(node);

            CNodePtr next_arg = arg->next_node;
            arg->next_node = nullptr;

            CNodePtr command = nullptr;
            if (i + 1 == arg_list.size() && LastAgumentInCpuRegister(*node->variable)) {
                command = CNODE(CNT_SAVE_TO_REGISTER, a : arg, ctype : arg_list[i].type, e : arg->e);
            } else {
                CVariablePtr &av = arg_variables[i - 1];
                assert(av != nullptr);

                CNodePtr aa = CNODE(CNT_CONST, ctype
                                    : arg_list[i].type, text
                                    : av->output_name, e
                                    : arg->e);  // TODO: Replace with CNT_LOAD_VARIABLE
                aa->ctype.pointers.push_back(CPointer{0});

                assert(!aa->text.empty());

                CNodePtr a = CNODE(CNT_MONO_OPERATOR, a
                                   : aa, ctype
                                   : arg_list[i].type, mono_operator_code
                                   : MOP_DEADDR, variable
                                   : av, e
                                   : arg->e);

                command = MakeOperator(COP_SET, a, arg, arg->e, p.programm.cmm);
            }
            body.PushBack(command);

            arg = next_arg;
        }

        // Extra arguments
        if (arg != nullptr)
            C_THROW_TYPE_NOT_SUPPORTED_INTERNAL(node);

        node->c = body.first;
        return true;
    }
    return false;
}

static void PrepareLastFunctionArgCode(Prepare &p) {
    if (p.programm.cmm)
        return;

    CVariable &f = *p.function;
    if (f.type.GetVariableMode() == CVM_GLOBAL && LastAgumentInCpuRegister(f)) {
        CVariablePtr &register_argument = p.function->function_arguments.back();
        if (p.programm.asm_names.find(register_argument->output_name) == p.programm.asm_names.end()) {
            CNodePtr a = CNODE(CNT_OPERATOR, next_node : p.function->body->a);
            p.function->body->a = a;
            a->e = p.function->e;
            a->operator_code = COP_SET;
            a->a = CNODE(CNT_LOAD_VARIABLE);
            a->a->variable = register_argument;
            a->a->ctype = register_argument->type;
            a->b = CNODE(CNT_LOAD_FROM_REGISTER);
            a->b->ctype = register_argument->type;
            a->ctype = register_argument->type;
        }
    }
}

void CalculateStaticStack(CProgramm &p) {
    if (p.c.static_stack == nullptr)
        return;  // There are no functions using a static stack

    // Calculate stack frame position for each function (by call tree)
    for (;;) {
        bool done = true;
        for (auto &i : p.all_top_variables) {
            if (i->c.use_counter > 0 && i->type.IsFunction() && i->c.call_count == 0 && i->c.called_by.size() > 0 &&
                !i->c.called_by_processed) {
                const size_t stack_end = i->c.function_stack_frame_offset + i->function_stack_frame_size;
                for (const auto &j : i->c.called_by) {
                    auto c = j.function.lock();

                    // Calculate stack frame offset
                    if (c->c.function_stack_frame_offset < stack_end)
                        c->c.function_stack_frame_offset = stack_end;

                    assert(c->c.call_count > 0);
                    if (c->c.call_count > 0) {
                        c->c.call_count--;
                        if (c->c.call_count == 0)
                            done = false;
                    }
                }
                i->c.called_by_processed = true;
            }
        }
        if (done)
            break;
    }

    size_t stack_size = 0;
    for (auto &x : p.all_top_variables) {
        if (x->type.IsFunction() && x->c.static_stack != nullptr) {
            // Check recursion
            if (!x->c.called_by_processed && x->c.called_by.size() > 0) {
                CVariablePtr v = x->c.called_by[0].function.lock();
                CThrow(x->e, "Recursion is not supported (" + x->name + " called by " + (v ? v->name : nullptr) + ")");
            }

            // Update function static_stack varaible
            x->c.static_stack->c.equ_enabled = true;  // TODO: Replace with CNT_LOAD_VARIABLE
            x->c.static_stack->c.equ_text =
                p.c.static_stack->output_name + " + " + std::to_string(x->c.function_stack_frame_offset);

            // Calculate stack size
            const size_t stack_end = x->c.function_stack_frame_offset + x->function_stack_frame_size;
            if (stack_size < stack_end)
                stack_size = stack_end;
        }
    }

    p.c.static_stack->type.pointers[0].count = stack_size;
}

void PrepareFunction(Prepare &p) {
    PrepareLastFunctionArgCode(p);
    PrepareFunctionStaticStack(p, *p.function);
}
