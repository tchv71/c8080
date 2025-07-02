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
#include "../asm/8080_assembler_optimize.h"
#include "../../prepare/staticstack.h"
#include "../prepare/prepare.h"
#include "../CompileVariable.h"

void Compiler8080::Compile(CParser &c, OutputFormat8080 output_format, CString output_file_bin, CString asm_file_name) {
    o.Init(p);  // Find internal functions

    static const char *main_function_name = "main";
    CVariablePtr main = p.FindVariable(main_function_name);
    if (main == nullptr || !main->type.IsFunction() || main->only_extern)
        throw std::runtime_error(std::string("function ") + main_function_name + " not found");
    out.AddToCompileQueue(main);

    for (size_t i = 0; i < out.compile_queue.size(); i++) {
        CVariablePtr fn = out.compile_queue[i];

        if (!fn->body)
            C_ERROR_INTERNAL(fn->e, "body is null");

        Prepare8080Function(p, fn->body, out);

        out.label(fn->output_name.c_str());

        // In the first function, clear BSS
        if (i == 0)
            out.assembler(c.LoadGlobalIncludeFile("asm/__init"));

        out.source(fn->body->e, true);

        return_label = nullptr;

        // Alloc stack
        if (fn->type.GetVariableMode() == CVM_STACK && fn->function_stack_frame_size != 0) {
            OutSubSpN(fn->function_stack_frame_size);
            return_label = out.AllocLabel();
        }
        out.stack_correction_reset(2 + fn->function_stack_frame_size);

        // Body
        current_function = fn;
        for (CNodePtr *i = &fn->body->a; *i != nullptr; i = &((*i)->next_node))
            CompileCommand(*i);
        current_function = nullptr;

        // Free stack
        if (return_label) {
            out.label(return_label);
            assert(fn->type.function_args.size() >= 1);
            AsmRegister result_reg = GetResultReg(fn->type.function_args[0].type, false, false, fn->body);
            OutAddSpN(result_reg, fn->function_stack_frame_size);
        }

        out.ret();
    }

    CalculateStaticStack(p);

    Assembler8080Optimize(out);

    // *** Make assembler file ***

    out.buffer =
        "    device zxspectrum48 ; It has nothing to do with ZX Spectrum 48K, it is needed for the sjasmplus "
        "compiler.\n";

    if (output_format == OF_I1080) {
        out.buffer += "    org 100h - 16h\n";
        out.buffer += "__begin:\n";  // TODO: Output name
        out.buffer +=
            "    db \"ISKRA1080\", 0xD0, \"A.OUT \", __entry, __entry >> 8, __end, __end >> 8, __entry, __entry >> 8\n";
        // TODO: Имя output_file_asm
        out.buffer += "__entry:\n";
    } else if (output_format == OF_CPM) {
        out.buffer += "    org 100h\n";
        out.buffer += "__begin:\n";
        out.buffer += "__entry:\n";
    }

    // Global assembler blocks
    for (CNodePtr *i = &p.first_node; *i != nullptr; i = &((*i)->next_node))
        if ((*i)->type == CNT_ASM)
            out.Write2((*i)->text);

    // Write assembler code
    out.MakeFile();

    // Write const strings
    for (auto &i : p.const_strings)
        if (i.second->c.IsUsed())
            out.const_string(i.second->c.GetName(out.const_string_counter), i.first.c_str());

    // Write user variables with non-zero value
    for (auto &vd : p.all_top_variables)
        if (vd->c.use_counter > 0 && !vd->only_extern && !vd->type.IsFunction() && !vd->c.equ_enabled && vd->body)
            CompileVariable(out, p, vd);

    // Write user variables with zero value
    out.buffer += "__bss:\n";
    for (auto &vd : p.all_top_variables) {
        if (vd->c.use_counter > 0 && !vd->only_extern && !vd->type.IsFunction() && !vd->c.equ_enabled &&
            vd->body == nullptr) {
            assert(!vd->address_attribute.exists);
            out.variable(vd->output_name.c_str());
            out.ds(vd->type.SizeOf(vd->e));
        }
    }
    out.buffer += "__end:\n";

    // Write equs, variables in static static
    for (auto &vd : p.all_top_variables)
        if (vd->c.use_counter > 0 && !vd->only_extern && !vd->type.IsFunction() && vd->c.equ_enabled)
            if (p.asm_names.find(vd->output_name) == p.asm_names.end())
                out.equ(vd->output_name.c_str(), vd->c.equ_text.c_str());

    // Save directive // TODO: Quote name
    out.buffer += "    savebin \"" + output_file_bin + "\", __begin, __bss - __begin\n";

    out.SaveFile(asm_file_name);
}
