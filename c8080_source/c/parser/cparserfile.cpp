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

void CParserFile::Compile(CNodeList &node_list, CString file_name) {
    p.save_string = [this](const char *data, size_t size) {
        return programm.SaveString(data, size);
    };

    p.preprocessor = [this](CString directive) {
        Preprocessor(directive);
    };

    const char *name = "";
    const char *contents = cparser.LoadFile(file_name, &name);
    p.Open(contents, name);
    p.AddMacro("__C8080_COMPILER", "", 0);
    for (auto &i : cparser.default_defines)
        p.AddMacro(i); // TODO: value
    p.NextToken();

    while (!p.IfToken(CT_EOF))
        node_list.PushBack(CompileLine(nullptr, true));
}

uint64_t CParserFile::ParseUint64() {
    static const CType uint64_ctype{CBT_UNSIGNED_LONG_LONG};
    CNodePtr value = Convert(uint64_ctype, ParseExpression());
    CCalcConst(value);
    if (value->type != CNT_NUMBER) {
        programm.Error(value->e, "Is not number");
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
        programm.Error(value->e, "Is not number");
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
        programm.AddVariable(label);
    }
    if (is_goto) {
        label->label_call_count++;
    } else {
        if (!label->only_extern)
            programm.Error(e, "duplicate label '" + name + "'");  // gcc
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

    CParseAsmEqus(str, programm.asm_names);

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
    auto &global_map = is_union ? programm.global_unions : programm.global_structs;
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
        programm.Error(e, "unsupported symbol at position " + std::to_string(pos) + " in string \"" + in + "\"");
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
            programm.Error(e, std::string("redefinition of ‘") + name + "’"); // gcc
        if (p.IfToken("="))
            value = ParseInt64();

        if (value < C_INT_MIN || value > C_INT_MAX)
            programm.Error(e, "overflow in enumeration values"); // gcc

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

CNodePtr CParserFile::ErrorContinue(CErrorPosition &e, CString text) // TODO: Move to parser
{
    programm.Error(e, text);
    while (!p.IfToken(";"))
        p.NextToken();
    return nullptr;
}

void CParserFile::ParseAttributes(CNode& n) {
    for (;;) {
        CErrorPosition e(p);
        if (p.IfToken("__address")) {
            CAddressAttribute a;

            p.NeedToken("(");
            a.value = ParseUint64();
            p.NeedToken(")");

            a.exists = true;

            if ((n.address_attribute.exists && n.address_attribute != a) ||
            (n.variable && n.variable->address_attribute.exists && n.variable->address_attribute != a))
                programm.Error(e, "previous declaration is different");

            n.address_attribute = a;
            if (n.variable)
                n.variable->address_attribute = a;
            continue;
        }
        if (p.IfToken("__link")) {
            CLinkAttribute a;

            p.NeedToken("(");
            p.NeedString2(a.base_name);
            p.NeedToken(")");

            a.name_for_path = p.file_name;
            a.exists = true;

            if ((n.link_attribute.exists && n.link_attribute != a) ||
                (n.variable && n.variable->link_attribute.exists && n.variable->link_attribute != a))
                programm.Error(e, "previous declaration is different");

            n.link_attribute = a;
            if (n.variable)
                n.variable->link_attribute = a;
            continue;
        }
        break;
    }
}

CNodePtr CParserFile::CompileLine(bool *out_break, bool global) {
    CErrorPosition e(p);

    if (p.IfToken(";"))
        return nullptr;

    if (p.IfToken("asm"))
        return ParseAsm(e);

    bool typedef_flag = p.IfToken("typedef");
    bool extern_flag = p.IfToken("extern");

    if (typedef_flag && extern_flag) {
        programm.Error(e, "multiple storage classes in declaration specifiers"); // gcc
        extern_flag = false;
    }

    CType base_type;
    if (!ParseTypeWoPointers(&base_type, out_break != nullptr)) {
        *out_break = true;
        if (typedef_flag || extern_flag)
            programm.Error(e, "useless storage class specifier in empty declaration"); // gcc
        return nullptr;
    }

    if (p.IfToken(";")) {
        if (typedef_flag || extern_flag)
            programm.Error(e, "useless storage class specifier in empty declaration"); // gcc
        return nullptr; // new struct, union, enum
    }

    CNodeList node_list;
    for (;;) {
        CType type;
        std::string name;
        ParseTypeNameArray(base_type, &name, type);

        IgnoreAttributes();

        CNodePtr node = CNODE(typedef_flag ? CNT_TYPEDEF : CNT_DECLARE_VARIABLE, ctype : type, extern_flag : extern_flag, e : e);

        bool is_function = type.base_type == CBT_FUNCTION && !type.IsPointer();
        if (is_function)
            node->extern_flag = true; // Function prototype is always "extern"

        CNodePtr init;
        if (p.IfToken("=")) {
            if (typedef_flag || is_function)
                p.SyntaxError();
            init = CompileInitBlock(node->ctype);
            CCalcConst(init);
        }

        CVariablePtr v = RegisterVariable(typedef_flag, node->extern_flag, node, global, name, e);

        if (v) {
            node->variable = v;
            AllocateObjectsForFunctionArgs(node);
            if (init)
                v->body = init;
        }

        ParseAttributes(*node);

        node_list.PushBack(node);

        if (p.IfToken("{")) {
            if (!is_function || typedef_flag)
                p.SyntaxError();
            if (!global)
                p.Throw("nested functions are not supported"); // TODO
            ParseFunction(node);
            break;
        }

        if (p.IfToken(";"))
            break;

        p.NeedToken(",");
    }

    return node_list.first;
}

void CParserFile::ParseTypePointers(CType &out_type) {
    while (p.IfToken("*")) {
        CPointer pointer;
        ParsePointerFlags(pointer);
        out_type.pointers.push_back(pointer);
    }
    for (;;) {
        if (out_type.variables_mode == CVM_DEFAULT && p.IfToken("__global")) {
            out_type.variables_mode = CVM_GLOBAL;
            continue;
        }
        if (out_type.variables_mode == CVM_DEFAULT && p.IfToken("__stack")) {
            out_type.variables_mode = CVM_STACK;
            continue;
        }
        break;
    }
}

void CParserFile::ParseTypeNameArray(CConstType base_type, std::string *out_name, CType& out_type) {
    CType type = base_type;
    ParseTypePointers(type);

    if (out_name)
        p.IfIdent(*out_name);

    if (p.IfToken("(")) {
        CErrorPosition e(p);
        if ((!out_name || out_name->empty()) && p.IfToken("*")) {
            std::vector<CPointer> pointers;
            do {
                CPointer pointer;
                ParsePointerFlags(pointer);
                pointers.push_back(pointer);
            } while(p.IfToken("*"));
            if (out_name)
                p.IfIdent(*out_name);
            p.NeedToken(")");
            if (p.IfToken("(")) {
                ParseFunctionTypeArgs(e, type, &pointers, out_type);
                return;
            }
            type.pointers.insert(type.pointers.end(), pointers.begin(), pointers.end());
        } else {
            ParseFunctionTypeArgs(e, type, nullptr, out_type);
            return;
        }
    }

    std::vector<size_t> addrs;
    while (p.IfToken("[")) {
        uint64_t size = 1;
        if (!p.IfToken("]")) {
            size = ParseUint64();
            p.NeedToken("]");
        }
        addrs.push_back(size);
    }
    for (size_t i = addrs.size(); i != 0; i--)
        type.pointers.push_back(CPointer{addrs[i - 1]});
    out_type = type;
}

void CParserFile::ParseFunctionTypeArgs(CErrorPosition& e, CType& return_type, std::vector<CPointer> *fp, CType& out_type) {
    out_type.base_type = CBT_FUNCTION;
    if (return_type.variables_mode != CVM_DEFAULT) {
        if (out_type.variables_mode != CVM_DEFAULT && out_type.variables_mode != return_type.variables_mode)
            programm.Error(e, "previous declaration is different");
        out_type.variables_mode = return_type.variables_mode;
    }
    return_type.variables_mode = CVM_DEFAULT;
    if (fp)
        for (auto &i : *fp)
            out_type.pointers.push_back(i);
    out_type.function_args.push_back(CStructItem{return_type});

    if (p.IfToken(")")) {
        out_type.many_function_args = true;
        return;
    }

    for (;;) {
        if (p.IfToken("...")) {
            p.NeedToken(")");
            out_type.many_function_args = true;
            break;
        }

        CErrorPosition e(p);
        CStructItem arg;
        CType pre_type;
        ParseType(&pre_type);

        if (pre_type.IsVoid()) {
            if (out_type.function_args.size() != 1)
                programm.Error(e, "'void' must be the only parameter"); // gcc
            p.NeedToken(")");
            out_type.many_function_args = false;
            break;
        }

        ParseTypeNameArray(pre_type, &arg.name, arg.type);

        out_type.function_args.push_back(arg);

        if (p.IfToken(")")) {
            out_type.many_function_args = false;
            break;
        }

        p.NeedToken(",");
    }
}
