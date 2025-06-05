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

#include "c_parser_file.h"
#include "ccalcconst.h"
#include "../tools/convert.h"
#include "../tools/cthrow.h"

uint64_t CParserFile::ParseUint64() {
    static const CType uint64_ctype{CBT_UNSIGNED_LONG_LONG};
    CNodePtr value = Convert(uint64_ctype, ParseExpression());
    CCalcConst(value);
    if (value->type != CNT_NUMBER)
        CThrow(value, "Is not number");
    assert(!value->ctype.IsPointer() && value->ctype.base_type == CBT_UNSIGNED_LONG_LONG);
    return value->number.u;
}

int64_t CParserFile::ParseInt64() {
    static const CType int64_ctype{CBT_LONG_LONG};
    CNodePtr value = Convert(int64_ctype, ParseExpression());
    CCalcConst(value);
    if (value->type != CNT_NUMBER)
        CThrow(value, "Is not number");
    assert(!value->ctype.IsPointer() && value->ctype.base_type == CBT_LONG_LONG);
    return value->number.i;
}

CVariablePtr CParserFile::BindLabel(CString name, CErrorPosition &e, bool is_goto) {
    CVariablePtr &label = scope_labels[name];
    if (!label) {
        label = std::make_shared<CVariable>();
        label->name = name;
        label->only_extern = true;
        label->is_label = true;
        label->output_name = name;
        label->e = e;
        programm_->AddVariable(label);
    }
    if (is_goto) {
        label->label_call_count++;
    } else {
        if (!label->only_extern)
            p.Throw("duplicate label '" + name + "'"); // gcc
        label->only_extern = false;
    }
    return label;
}
