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
#include "../consts.h"
#include "../tools/ccalcconst.h"
#include "../tools/convert.h"
#include "../tools/cthrow.h"
#include "../tools/makeoperator.h"

void CParserFile::Compile(CNodeList &node_list, CString file_name) {
    p.save_string = [this](const char *data, size_t size) { return programm.SaveString(data, size); };

    p.preprocessor = [this](CString directive) { Preprocessor(directive); };

    const char *name = "";
    const char *contents = cparser.LoadFile(file_name, &name);
    p.Open(contents, name);
    p.AddMacro("__C8080_COMPILER", "", 0);
    for (auto &i : cparser.default_defines)
        p.AddMacro(i);  // TODO: value
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

CStructPtr CParserFile::BindStructUnion(CString name, bool is_union, bool is_global, CErrorPosition &e) {
    auto &scope = is_union ? scope_unions : scope_structs;
    auto &global_map = is_union ? programm.global_unions : programm.global_structs;

    if (!name.empty()) {
        // Check unique name in view scope
        for (auto i = scope.rbegin(); i != scope.rend(); i++)  // TODO: Use map
            if ((*i)->name == name)
                return (*i);

        // To check compatibility of types in different units
        if (is_global) {
            auto i = global_map.find(name);
            if (i != global_map.end())
                return i->second;
        }
    }

    // Allocate struct
    CStructPtr s = std::make_shared<CStruct>();
    s->e = e;
    s->is_union = is_union;

    if (!name.empty()) {
        s->name = name;

        scope.push_back(s);

        // To check compatibility of types in different units
        if (is_global)
            global_map[name] = s;
    }

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

void CParserFile::Utf8To8Bit(const CErrorPosition &e, CString in, std::string &out) {
    size_t pos = ::Utf8To8Bit(codepage, in, out);
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
            programm.Error(e, std::string("redefinition of ‘") + name + "’");  // gcc
        if (p.IfToken("="))
            value = ParseInt64();

        if (value < C_INT_MIN || value > C_INT_MAX)
            programm.Error(e, "overflow in enumeration values");  // gcc

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

CNodePtr CParserFile::ErrorContinue(CErrorPosition &e, CString text)  // TODO: Move to parser
{
    programm.Error(e, text);
    while (!p.IfToken(";"))
        p.NextToken();
    return nullptr;
}

void CParserFile::ParseAttributes(CNode &n) {
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
        programm.Error(e, "multiple storage classes in declaration specifiers");  // gcc
        extern_flag = false;
    }

    CType base_type;
    if (!ParseTypeWoPointers(&base_type, out_break != nullptr)) {
        *out_break = true;
        if (typedef_flag || extern_flag)
            programm.Error(e, "useless storage class specifier in empty declaration");  // gcc
        return nullptr;
    }

    if (typedef_flag && base_type.flag_static) {
        programm.Error(e, "multiple storage classes in declaration specifiers");  // gcc
        base_type.flag_static = false;
    }

    if (p.IfToken(";")) {
        if (typedef_flag || extern_flag)
            programm.Error(e, "useless storage class specifier in empty declaration");  // gcc
        return nullptr;                                                                 // new struct, union, enum
    }

    CNodeList node_list;
    for (;;) {
        CType type;
        std::string name;
        ParseTypeNameArray(base_type, name, type);

        if (typedef_flag) {
            CNodePtr node = CNODE(CNT_TYPEDEF, ctype : type, e : e);
            RegisterTypedef(node, name);

            if (p.IfToken(";"))
                break;

            p.NeedToken(",");
            continue;
        }

        IgnoreAttributes();

        CNodePtr node = CNODE(typedef_flag ? CNT_TYPEDEF : CNT_DECLARE_VARIABLE, ctype
                              : type, extern_flag
                              : extern_flag, e
                              : e);

        bool is_function = type.base_type == CBT_FUNCTION && !type.IsPointer();
        if (is_function)
            node->extern_flag = true;  // Function prototype is always "extern"

        CNodePtr init;
        if (p.IfToken("=")) {
            if (typedef_flag || is_function)
                p.SyntaxError();
            init = CompileInitBlock(node->ctype);
            CCalcConst(init);
        }

        CVariablePtr v = RegisterVariable(node->extern_flag, node, global, name);

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
                p.Throw("nested functions are not supported");  // TODO
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

void CParserFile::ParseTypeNameArray(CConstType base_type, std::string &out_name, CType &out_type) {
    CType type = base_type;
    ParseTypePointers(type);

    p.IfIdent(out_name);

    if (p.IfToken("(")) {
        CErrorPosition e(p);
        if (out_name.empty() && p.IfToken("*")) {
            std::vector<CPointer> pointers;
            do {
                CPointer pointer;
                ParsePointerFlags(pointer);
                pointers.push_back(pointer);
            } while (p.IfToken("*"));
            p.IfIdent(out_name);
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

void CParserFile::ParseFunctionTypeArgs(CErrorPosition &e, CType &return_type, std::vector<CPointer> *fp,
                                        CType &out_type) {
    out_type.base_type = CBT_FUNCTION;
    out_type.flag_static = return_type.flag_static;
    return_type.flag_static = false;
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
                programm.Error(e, "'void' must be the only parameter");  // gcc
            p.NeedToken(")");
            out_type.many_function_args = false;
            break;
        }

        ParseTypeNameArray(pre_type, arg.name, arg.type);

        out_type.function_args.push_back(arg);

        if (p.IfToken(")")) {
            out_type.many_function_args = false;
            break;
        }

        p.NeedToken(",");
    }
}

void CParserFile::ParseStruct(CStruct &struct_object) {
    CErrorPosition e(p);
    while (!p.IfToken("}")) {
        CType base_type;
        ParseTypeWoPointers(&base_type);
        do {
            CStructItemPtr struct_item = std::make_shared<CStructItem>();
            struct_item->type = base_type;
            ParseTypeNameArray(base_type, struct_item->name, struct_item->type);
            IgnoreAttributes();
            struct_object.items.push_back(struct_item);
        } while (p.IfToken(","));
        p.NeedToken(";");
    }
    struct_object.CalcOffsets(e);
}

bool CParserFile::ParseType(CType *out_type, bool can_empty) {
    if (!ParseTypeWoPointers(out_type, can_empty))
        return false;
    ParseTypePointers(*out_type);
    return true;
}

void CParserFile::Enter() {
    prev_scopes.push_back(Level{scope_variables.size(), scope_structs.size(), scope_unions.size(),
                                scope_typedefs.size(), current_stack_size});
}

void CParserFile::Leave() {
    if (prev_scopes.empty())  // TODO: Make special object scoped_map
        p.Throw(std::string("Internal error in ") + __PRETTY_FUNCTION__);
    Level &level = prev_scopes.back();
    scope_variables.resize(level.variables_count);
    scope_structs.resize(level.structs_count);
    scope_unions.resize(level.unions_count);
    scope_typedefs.resize(level.typedefs_count);
    current_stack_size = level.stack_size;
    prev_scopes.pop_back();
}

void CParserFile::ParseFunction(CNodePtr &node) {
    if (!node->variable->only_extern) {
        programm.Error(node->e, "redefinition of '" + node->variable->name + "'");                  // gcc
        programm.Note(node->variable->e, "previous definition of '" + node->variable->name + "'");  // gcc
    }

    node->extern_flag = false;
    node->variable->only_extern = false;
    node->variable->body = node;

    // Init variables
    current_function = node->variable;
    scope_labels.clear();
    current_stack_size = 0;
    max_stack_size = 0;

    Enter();

    if (node->ctype.function_args.size() != current_function->function_arguments.size() + 1)
        throw std::runtime_error(std::string("Internal error in ") + __PRETTY_FUNCTION__);

    for (size_t i = 0; i < current_function->function_arguments.size(); i++) {
        CVariablePtr &a = current_function->function_arguments[i];
        a->name = node->ctype.function_args[i + 1u].name;
        scope_variables.push_back(a);
    }

    CNodeList list;
    while (!p.IfToken("}"))
        list.PushBack(ParseFunctionBody());
    node->a = list.first;

    Leave();

    current_function->function_stack_size = max_stack_size;

    // Check labels
    for (auto &l : scope_labels)
        if (l.second->only_extern)
            CThrow(l.second->e, "label '" + l.second->name + "'  used but not defined");  // gcc

    // Self check
    current_function = nullptr;
}

void CParserFile::AllocateObjectsForFunctionArgs(CNodePtr node) {
    assert(node && node->variable);

    CVariable &v = *node->variable;

    if (!v.type.IsFunction())
        return;

    if (!v.function_arguments.empty())
        return;  // Already created

    std::vector<CStructItem *> args;
    if (v.type.GetVariableMode() == CVM_GLOBAL) {
        for (size_t i = node->ctype.function_args.size(); i > 1; i--)
            args.push_back(&node->ctype.function_args[i - 1]);
    } else {
        for (size_t i = 1; i < node->ctype.function_args.size(); i++)
            args.push_back(&node->ctype.function_args[i]);
    }

    std::map<std::string, int> names;

    uint64_t offset = 0;
    for (auto &item : args) {
        if (!names.try_emplace(item->name, 0).second)
            programm.Error(node->e, std::string("redefinition of parameter '") + item->name + "'");  // gcc

        CVariablePtr a = std::make_shared<CVariable>();
        a->type = item->type;
        a->name = item->name;
        a->is_stack_variable = true;
        a->is_function_argument = true;
        a->e = node->e;

        auto argument_size = a->type.SizeOf(a->e);
        if (argument_size == 1)
            offset++;  // The stack is word aligned
        a->stack_offset = offset;
        offset += argument_size;

        if (v.type.GetVariableMode() == CVM_GLOBAL)
            v.function_arguments.insert(v.function_arguments.begin(), a);
        else
            v.function_arguments.push_back(a);
    }
}

void CParserFile::RegisterTypedef(CNodePtr node, CString name) {
    CTypedef *b = FindTypedefCurrentScope(name);
    if (b != nullptr) {
        if (b->type != node->ctype) {
            programm.Error(node->e,
                           "conflicting types for '" + name + "'; have '" + node->ctype.ToString() + "'");  // gcc
            programm.Note(b->e,
                          "previous declaration of '" + name + "' with type '" + b->type.ToString() + "'");  // gcc
        }
    } else {
        scope_typedefs.push_back(CTypedef{node->ctype, name, e : node->e});
    }
}

CVariablePtr CParserFile::RegisterVariable(bool extern_flag, CNodePtr node, bool global, CString name) {
    CVariablePtr v = FindVariableCurrentScope(name);
    if (v != nullptr) {
        if (v->type != node->ctype) {
            programm.Error(node->e,
                           "conflicting types for '" + name + "'; have '" + node->ctype.ToString() + "'");  // gcc
            programm.Note(v->e,
                          "previous declaration of '" + name + "' with type '" + v->type.ToString() + "'");  // gcc
        }
        if (!extern_flag && !v->only_extern) {
            programm.Error(node->e, "redefinition of '" + name + "'");     // gcc
            programm.Note(v->e, "previous definition of '" + name + "'");  // gcc
        }
        if (!extern_flag)
            v->only_extern = false;
        // TODO: void test() { extern int a; int a; }
        return v;
    }

    if (extern_flag)
        global = true;

    if (global && !node->ctype.flag_static) {
        v = programm.global_variables[name];
        if (v != nullptr) {
            if (node->ctype != v->type) {
                programm.Error(node->e,
                               "conflicting types for '" + name + "'; have '" + node->ctype.ToString() + "'");  // gcc
                programm.Note(v->e,
                              "previous declaration of '" + name + "' with type '" + v->type.ToString() + "'");  // gcc
            }
            if (!extern_flag && !v->only_extern) {
                programm.Error(node->e, "redefinition of '" + name + "'");     // gcc
                programm.Note(v->e, "previous definition of '" + name + "'");  // gcc
            }
            if (!extern_flag)
                v->only_extern = false;
            scope_variables.push_back(v);
            return v;
        }
    }

    v = std::make_shared<CVariable>();
    v->e = node->e;
    v->type = node->ctype;
    v->name = name;
    v->only_extern = extern_flag;
    v->address_attribute = node->address_attribute;
    v->link_attribute = node->link_attribute;
    if (global) {
        if (!node->ctype.flag_static)
            programm.global_variables[name] = v;
        v->output_name = name;
        programm.AddVariable(v);
    } else if (node->ctype.flag_static) {
        v->output_name = "__s_" + current_function->name + "_" + name;
        programm.AddVariable(v);
    } else {
        v->is_stack_variable = true;
        v->stack_offset = current_stack_size;

        current_stack_size += v->type.SizeOf(v->e);
        if (max_stack_size < current_stack_size)
            max_stack_size = current_stack_size;
    }
    scope_variables.push_back(v);
    return v;
}

void CParserFile::ParseTypeWoPointersStruct(CType *out_type, bool is_union, CErrorPosition &e) {
    out_type->base_type = CBT_STRUCT;

    std::string name;
    if (p.IfIdent(name)) {
        out_type->struct_object = BindStructUnion(name, is_union, true, e);
        CStruct &s = *out_type->struct_object;

        if (p.IfToken("{")) {
            CStruct l;
            l.is_union = is_union;
            l.name = name;
            l.e = e;
            ParseStruct(l);

            if (s.inited) {
                if (s != l) {
                    programm.Error(l.e, "conflicting types for '" + name + "'; have '" + l.ToString() + "'");  // gcc
                    programm.Note(s.e,
                                  "previous declaration of '" + name + "' with type '" + s.ToString() + "'");  // gcc
                }
            } else {
                s = l;
            }
        }
    } else {
        p.NeedToken("{");
        out_type->struct_object = BindStructUnion("", is_union, true, e);
        ParseStruct(*out_type->struct_object);
    }
}

CNodePtr CParserFile::ParseExpressionComma() {
    CNodePtr result = ParseExpression();
    for (;;) {
        CErrorPosition e(p);
        if (!p.IfToken(","))
            return result;
        result = MakeOperator(COP_COMMA, result, ParseExpression(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpression() {
    CNodePtr a = ParseExpressionA();

    CErrorPosition e(p);
    if (p.IfToken("?")) {
        CNodePtr b = ParseExpression();
        p.NeedToken(":");
        CNodePtr c = ParseExpression();
        return MakeOperatorIf(a, b, c, e, programm.cmm);
    }

    static const char *const operators[] = {"=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=", nullptr};
    static const COperatorCode operator_codes[] = {COP_SET,     COP_SET_ADD, COP_SET_SUB, COP_SET_MUL,
                                                   COP_SET_DIV, COP_SET_MOD, COP_SET_SHL, COP_SET_SHR,
                                                   COP_SET_AND, COP_SET_XOR, COP_SET_OR};
    size_t n = 0;
    if (p.IfToken(operators, n))
        return MakeOperator(operator_codes[n], a, ParseExpression(), e, programm.cmm);

    return a;
}

CNodePtr CParserFile::ParseExpressionA() {
    CNodePtr result = ParseExpressionB();
    for (;;) {
        CErrorPosition e(p);
        if (!p.IfToken("||"))
            return result;
        result = MakeOperator(COP_LOR, result, ParseExpressionB(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionB() {
    CNodePtr result = ParseExpressionC();
    for (;;) {
        CErrorPosition e(p);
        if (!p.IfToken("&&"))
            return result;
        result = MakeOperator(COP_LAND, result, ParseExpressionC(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionC() {
    CNodePtr result = ParseExpressionD();
    for (;;) {
        CErrorPosition e(p);
        if (!p.IfToken("|"))
            return result;
        result = MakeOperator(COP_OR, result, ParseExpressionD(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionD() {
    CNodePtr result = ParseExpressionE();
    for (;;) {
        CErrorPosition e(p);
        if (!p.IfToken("^"))
            return result;
        result = MakeOperator(COP_XOR, result, ParseExpressionE(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionE() {
    CNodePtr result = ParseExpressionF();
    for (;;) {
        CErrorPosition e(p);
        if (!p.IfToken("&"))
            return result;
        result = MakeOperator(COP_AND, result, ParseExpressionF(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionF() {
    static const char *const operators[] = {"==", "!=", nullptr};
    static const COperatorCode operator_codes[] = {COP_CMP_E, COP_CMP_NE};
    CNodePtr result = ParseExpressionG();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(p);
        if (!p.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionG(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionG() {
    static const char *const operators[] = {"<", "<=", ">", ">=", nullptr};
    static const COperatorCode operator_codes[] = {COP_CMP_L, COP_CMP_LE, COP_CMP_G, COP_CMP_GE};
    CNodePtr result = ParseExpressionH();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(p);
        if (!p.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionH(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionH() {
    static const char *const operators[] = {">>", "<<", nullptr};
    static const COperatorCode operator_codes[] = {COP_SHR, COP_SHL};
    CNodePtr result = ParseExpressionI();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(p);
        if (!p.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionI(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionI() {
    static const char *const operators[] = {"+", "-", nullptr};
    static const COperatorCode operator_codes[] = {COP_ADD, COP_SUB};
    CNodePtr result = ParseExpressionJ();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(p);
        if (!p.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionJ(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionJ() {
    static const char *const operators[] = {"*", "/", "%", nullptr};
    static const COperatorCode operator_codes[] = {COP_MUL, COP_DIV, COP_MOD};
    CNodePtr result = ParseExpressionK();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(p);
        if (!p.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionK(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionK() {
    CErrorPosition e(p);
    if (p.IfToken("(")) {
        CType new_type;
        if (ParseType(&new_type, true)) {
            p.NeedToken(")");
            return CNODE(CNT_CONVERT, a : ParseExpressionK(), ctype : new_type, e : e);
        }
        CNodePtr result = ParseExpressionComma();
        p.NeedToken(")");
        return ParseExpressionM(result);
    }

    static const char *const operators[] = {"++", "--", "+", "-", "!", "~", "*", "&", nullptr};
    static const CMonoOperatorCode operator_codes[] = {MOP_INC, MOP_DEC, MOP_PLUS,   MOP_MINUS,
                                                       MOP_NOT, MOP_NEG, MOP_DEADDR, MOP_ADDR};
    size_t n = 0;
    if (p.IfToken(operators, n)) {
        CNodePtr a = ParseExpressionK();
        CNodePtr result = CNODE(CNT_MONO_OPERATOR, a
                                : a, ctype
                                : a->ctype, mono_operator_code
                                : operator_codes[n], e
                                : e);
        if (operator_codes[n] == MOP_DEADDR) {
            if (result->ctype.pointers.empty()) {
                if (!programm.cmm)
                    programm.Error(
                        e, "invalid type argument of unary '*' (have '" + result->ctype.ToString() + "')");  // gcc
            } else {
                result->ctype.pointers.pop_back();
            }
            return result;
        }
        if (operator_codes[n] == MOP_ADDR) {
            result->ctype.pointers.push_back(CPointer{0});
            return result;
        }
        if (operator_codes[n] == MOP_NOT) {
            result->ctype = CType{CBT_BOOL};
            return result;
        }
        return result;
    }
    return ParseExpressionL();
}

CNodePtr CParserFile::ParseExpressionL() {
    CNodePtr result = ParseExpressionValue();
    return CParserFile::ParseExpressionM(result);
}
