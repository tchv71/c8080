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
#include "../../c/tools/numberiszero.h"

void Compiler8080::CompileJumpIf(CNodePtr &node, bool jmp_if_true, AsmLabel *label) {
    switch (node->type) {
        case CNT_MONO_OPERATOR:
            if (node->mono_operator_code == MOP_NOT)
                return CompileJumpIf(node->a, !jmp_if_true, label);
            break;
        case CNT_OPERATOR:
            if (node->operator_code == COP_CMP_E || node->operator_code == COP_CMP_NE) {
                if (NumberIsZero(node->b))
                    return CompileJumpIfZero(node->a, node->operator_code == COP_CMP_E, jmp_if_true, label);
                if (NumberIsZero(node->a))
                    return CompileJumpIfZero(node->b, node->operator_code == COP_CMP_E, jmp_if_true, label);
            }

            switch (node->operator_code) {
                case COP_CMP_E:
                case COP_CMP_NE:
                case COP_CMP_L:
                case COP_CMP_GE:
                    Build(node);
                    Build(node, R8_A);
                    switch (jmp_if_true ? node->operator_code : NegativeCompareOperator(node->operator_code)) {
                        case COP_CMP_E:
                            out.jz_label(label);
                            break;
                        case COP_CMP_NE:
                            out.jnz_label(label);
                            break;
                        case COP_CMP_L:
                            if (node->a->ctype.IsUnsigned())
                                out.jc_label(label);
                            else
                                out.jm_label(label);
                            break;
                        case COP_CMP_GE:
                            if (node->a->ctype.IsUnsigned())
                                out.jnc_label(label);
                            else
                                out.jp_label(label);
                            break;
                        default:
                            C_ERROR_UNSUPPORTED_OPERATOR(node);
                    }
                    return;
                case COP_LAND:
                    if (jmp_if_true) {
                        auto label2 = out.AllocLabel();
                        CompileJumpIf(node->a, false, label2);
                        CompileJumpIf(node->b, true, label);
                        out.label(label2);
                    } else {
                        CompileJumpIf(node->a, false, label);
                        CompileJumpIf(node->b, false, label);
                    }
                    return;
                case COP_LOR:
                    if (jmp_if_true) {
                        CompileJumpIf(node->a, true, label);
                        CompileJumpIf(node->b, true, label);
                    } else {
                        auto label2 = out.AllocLabel();
                        CompileJumpIf(node->a, true, label2);
                        CompileJumpIf(node->b, false, label);
                        out.label(label2);
                    }
                    return;
            }
    }
    C_ERROR_UNSUPPORTED_NODE_TYPE(node);
}
