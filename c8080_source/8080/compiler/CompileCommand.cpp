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

void Compiler8080::CompileCommand(CNodePtr &node) {
    if (node->e.cursor)
        out.source(node->e);

    switch (node->type) {
        case CNT_BREAK:
            if (break_label)
                out.jmp_label(break_label);
            else
                p.Error(node->e, "break statement not within loop or switch");  // gcc
            break;
        case CNT_CONTINUE:
            if (continue_label)
                out.jmp_label(continue_label);
            else
                p.Error(node->e, "continue statement not within a loop");  // gcc
            break;
        case CNT_DECLARE_VARIABLE:
            assert(node->variable != nullptr);
            assert(node->variable->body == nullptr || node->variable->type.flag_static);
            // Removed by optimizer
            break;
        case CNT_LEVEL:
            for (CNodePtr *i = &node->a; *i != nullptr; i = &((*i)->next_node))
                CompileCommand(*i);
            break;
        case CNT_RETURN:
            if (node->a) {
                Build(node->a);
                Build(node->a, GetResultReg(current_function->type.function_args[0].type, false, false, node));
            }
            if (return_label)
                out.jmp_label(return_label);
            else
                out.ret();
            break;
        case CNT_IF: {
            const auto label_false = out.AllocLabel();
            const auto label_exit = out.AllocLabel();
            BuildJumpIf(true, node->a, false, label_false);
            BuildJumpIf(false, node->a, false, label_false);
            if (node->b)
                CompileCommand(node->b);
            if (node->c)
                out.jmp_label(label_exit);
            out.label(label_false);
            if (node->c) {
                CompileCommand(node->c);
                out.label(label_exit);
            }
            break;
        }
        case CNT_SWITCH:
            CompileSwitch(node);
            break;
        case CNT_CASE:
            out.label(node->compiler.label);
            break;
        case CNT_DEFAULT:
            out.label(node->compiler.label);
            break;
        case CNT_FOR: {
            if (node->a)
                CompileCommand(node->a);
            const auto saved_continue_label = continue_label;
            const auto saved_break_label = break_label;
            const auto start_label = out.AllocLabel();
            continue_label = out.AllocLabel();
            break_label = out.AllocLabel();
            out.label(start_label);
            if (node->b) {
                BuildJumpIf(true, node->b, false, break_label);
                BuildJumpIf(false, node->b, false, break_label);
            }
            if (node->d)
                CompileCommand(node->d);
            out.label(continue_label);
            if (node->c)
                CompileCommand(node->c);
            out.jmp_label(start_label);
            out.label(break_label);
            continue_label = saved_continue_label;
            break_label = saved_break_label;
            break;
        }
        case CNT_WHILE: {
            const auto saved_continue_label = continue_label;
            const auto saved_break_label = break_label;
            continue_label = out.AllocLabel();
            break_label = out.AllocLabel();
            out.label(continue_label);
            if (node->a) {
                BuildJumpIf(true, node->a, false, break_label);
                BuildJumpIf(false, node->a, false, break_label);
            }
            if (node->b)
                CompileCommand(node->b);
            out.jmp_label(continue_label);
            out.label(break_label);
            continue_label = saved_continue_label;
            break_label = saved_break_label;
            break;
        }
        case CNT_ASM:
            out.assembler(node->text.c_str());
            break;
        case CNT_DO: {
            const auto saved_continue_label = continue_label;
            const auto saved_break_label = break_label;
            const auto start_label = out.AllocLabel();
            continue_label = out.AllocLabel();
            break_label = out.AllocLabel();
            out.label(start_label);
            CompileCommand(node->b);
            out.label(continue_label);
            BuildJumpIf(true, node->a, true, start_label);
            BuildJumpIf(false, node->a, true, start_label);
            out.label(break_label);
            continue_label = saved_continue_label;
            break_label = saved_break_label;
            break;
        }
        case CNT_SAVE_TO_REGISTER:
            Build(node->a);
            Build(node->a, GetResultReg(node->ctype, false, false, node));
            // Leave the result in the processor register
            break;
        default:
            Build(node);
            Build(node, REG_NONE);
    }
}
