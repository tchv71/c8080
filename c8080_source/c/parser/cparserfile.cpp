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
#include "cparseasmequs.h"
#include "../tools/ccalcconst.h"
#include "../tools/convert.h"
#include "../tools/cthrow.h"
#include "../consts.h"

uint64_t CParserFile::ParseUint64() {
    static const CType uint64_ctype{CBT_UNSIGNED_LONG_LONG};
    CNodePtr value = Convert(uint64_ctype, ParseExpression());
    CCalcConst(value);
    if (value->type != CNT_NUMBER) {
        programm_->Error(value->e, "Is not number");
        return 0;
    }
    assert(!value->ctype.IsPointer() && value->ctype.base_type == CBT_UNSIGNED_LONG_LONG);
    return value->number.u;
}

int64_t CParserFile::ParseInt64() {
    static const CType int64_ctype{CBT_LONG_LONG};
    CNodePtr value = Convert(int64_ctype, ParseExpression());
    CCalcConst(value);
    if (value->type != CNT_NUMBER) {
        programm_->Error(value->e, "Is not number");
        return 0;
    }
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
            programm_->Error(e, "duplicate label '" + name + "'");  // gcc
        label->only_extern = false;
    }
    return label;
}

void CParserFile::ParsePointerFlags(CPointer &pointer) {
    for (;;) {
        if (!pointer.flag_restrict && p.IfToken("__restrict")) {
            pointer.flag_restrict = true;
            continue;
        }
        if (!pointer.flag_const && p.IfToken("const")) {
            pointer.flag_const = true;
            continue;
        }
        if (!pointer.flag_volatile && p.IfToken("volatile")) {
            pointer.flag_volatile = true;
            continue;
        }
        break;
    }
}

void CParserFile::IgnoreInsideBrackets(size_t level) {
    for (;;) {
        if (p.IfToken("(")) {
            level++;
            if (level == 0)  // Incredible overflow
                p.SyntaxError();
            continue;
        }
        if (p.IfToken(")")) {
            level--;
            if (level == 0)
                break;
            continue;
        }
        p.NextToken();
    }
}

void CParserFile::IgnoreAttributes() {  // gcc compatibility
    for (;;) {
        if (p.IfToken("__asm__")) {
            p.NeedToken("(");
            IgnoreInsideBrackets(1);
            continue;
        }

        if (p.IfToken("__attribute__")) {
            p.NeedToken("(");
            p.NeedToken("(");
            IgnoreInsideBrackets(2);
            continue;
        }

        break;
    }
}

CNodePtr CParserFile::ParseAsm(CErrorPosition &e) {
    std::string str;
    if (p.IfToken("(")) {
        p.NeedString2(str);
        p.NeedToken(")");
        p.NeedToken(";");
    } else if (p.token_data[0] == '{') {
        p.ReadRaw(str, '}');
        p.NextToken();
    } else {
        p.SyntaxError();
    }

    CParseAsmEqus(str, programm_->asm_names);

    return CNODE(CNT_ASM, text : str, e : e);
}

CStructPtr CParserFile::BindStructUnion(CString name, bool is_union, bool is_global) {
    assert(!name.empty());

    // Check unique name in view scope
    auto &scope = is_union ? scope_unions : scope_structs;
    for (auto i = scope.rbegin(); i != scope.rend(); i++)  // TODO: Use map
        if ((*i)->name == name)
            return (*i);

    // To check compatibility of types in different units
    auto &global_map = is_union ? programm_->global_unions : programm_->global_structs;
    if (is_global) {
        auto i = global_map.find(name);
        if (i != global_map.end())
            return i->second;
    }

    // Allocate struct
    CStructPtr s = std::make_shared<CStruct>();
    s->is_union = is_union;
    s->name = name;
    scope.push_back(s);

    // To check compatibility of types in different units
    if (is_global)
        global_map[name] = s;

    return s;
}

CVariablePtr CParserFile::FindVariableCurrentScope(CString name) {
    const size_t start = prev_scopes.empty() ? 0 : prev_scopes.back().variables_count;
    for (size_t i = start; i < scope_variables.size(); i++)  // TODO: Use map
        if (scope_variables[i]->name == name)
            return scope_variables[i];
    return nullptr;
}

CTypedef *CParserFile::FindTypedefCurrentScope(CString name) {
    const size_t start = prev_scopes.empty() ? 0 : prev_scopes.back().typedefs_count;
    for (size_t i = start; i < scope_typedefs.size(); i++)  // TODO: Use map
        if (scope_typedefs[i].name == name)
            return &scope_typedefs[i];
    return nullptr;
}

void CParserFile::Utf8To8Bit(const CErrorPosition& e, CString in, std::string &out) {
    size_t pos = ::Utf8To8Bit(codepage_, in, out);
    if (pos != SIZE_MAX)
        programm_->Error(e, "unsupported symbol at position " + std::to_string(pos) + " in string \"" + in + "\"");
}

void CParserFile::ParseEnum() {
    // TODO: enum automatically increases the size of the type
    int64_t value = 0;
    for (;;) {
        if (p.IfToken("}"))
            break;

        CErrorPosition e(p);
        std::string name;
        p.NeedIdent(name);
        if (FindVariableCurrentScope(name) != nullptr)
            programm_->Error(e, std::string("redefinition of ‘") + name + "’"); // gcc
        if (p.IfToken("="))
            value = ParseInt64();

        if (value < C_INT_MIN || value > C_INT_MAX)
            programm_->Error(e, "overflow in enumeration values"); // gcc

        CVariablePtr v = std::make_shared<CVariable>();
        v->type.base_type = CBT_INT;
        v->type.flag_const = true;
        v->name = name;
        v->e = e;
        v->body = CNODE(CNT_NUMBER, ctype : CType{CBT_INT});
        v->body->number.i = value;

        scope_variables.push_back(v);

        if (p.IfToken("}"))
            break;
        p.NeedToken(",");
        value++;
    }
}
