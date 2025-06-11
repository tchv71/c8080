#pragma once

#include "clex.h"
#include "../cnodelist.h"
#include "cparserbase.h"

class CParserFile : public CParserBase {
private:
    CLex l;
    size_t loop_level{};
    CNodePtr last_switch;
    CVariablePtr current_function;
    std::map<std::string, int> pragma_once;

    CNodePtr ParseLine(bool *out_break, bool global);
    void ParseFunction(CNodePtr &node);
    void ParseAttributes(CNode &n);
    bool ParseTypeWoPointers(CType *out_type, bool can_break_inital = false);
    void ParseTypePointers(CType &out_type);
    void ParseTypeNameArray(CConstType base_type, std::string &out_name, CType &out_type);
    void ParsePointerFlags(CPointer &addr);
    void ParseFunctionTypeArgs(CErrorPosition &e, CType &return_type, std::vector<CPointer> *function_pointer,
                               CType &out_type);
    CNodePtr ParseInitBlock(CType &type, bool can_change_size);
    CNodePtr ParseAsm(CErrorPosition &e);
    CNodePtr ParseFunctionBody();
    CNodePtr ParseFunctionBody2();
    CNodePtr ParseExpressionValue();
    CNodePtr ParseExpressionL();
    CNodePtr ParseExpressionM(CNodePtr result);
    CNodePtr ParseExpressionK();
    CNodePtr ParseExpressionJ();
    CNodePtr ParseExpressionI();
    CNodePtr ParseExpressionH();
    CNodePtr ParseExpressionG();
    CNodePtr ParseExpressionF();
    CNodePtr ParseExpressionE();
    CNodePtr ParseExpressionD();
    CNodePtr ParseExpressionC();
    CNodePtr ParseExpressionB();
    CNodePtr ParseExpressionA();
    CNodePtr ParseExpression();
    CNodePtr ParseExpressionComma();
    void ParseStruct(CStruct &struct_object);
    bool ParseType(CType *out_type, bool can_empty = false);
    uint64_t ParseUint64();
    int64_t ParseInt64();
    void ParseEnum();
    void ParseTypeWoPointersStruct(CType *out_type, bool is_union, CErrorPosition &e);
    CNodePtr ParseExpressionCall(CNodePtr &f, CErrorPosition &e);
    CBaseType ParseBaseType();
    CNodePtr ParseExpressionStructItem(CMonoOperatorCode mo, CNodePtr &a, CErrorPosition &e);
    CNodePtr ParseExpressionArrayElement(CNodePtr &a, CErrorPosition &e);
    void IgnoreAttributes();
    void IgnoreInsideBrackets(size_t level);

    // Preprocessor

    void Preprocessor();
    void PreprocessorInclude();
    void PreprocessorPragmaCodepage();
    void PreprocessorPragmaOnce();
    void PreprocessorDefine();
    void PreprocessorIfdef();
    void PreprocessorIfndef();
    void PreprocessorUndef();
    void PreprocessorEndif();
    void PreprocessorElse();

public:
    CParserFile(CParser &p) : CParserBase(p) {
    }
    void Parse(CNodeList &node_list, CString file_name);
};
