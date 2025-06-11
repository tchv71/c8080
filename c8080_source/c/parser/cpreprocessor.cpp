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

#include "../../tools/trimself.h"
#include "../../tools/parseutf8.h"
#include "cparserfile.h"

void CParserFile::Preprocessor() {
    if (l.IfToken("include"))
        return PreprocessorInclude();
    if (l.IfToken("pragma")) {
        if (l.IfToken("codepage"))
            return PreprocessorPragmaCodepage();
        if (l.IfToken("once"))
            return PreprocessorPragmaOnce();
        // TODO: #pragma pack
        l.Error("invalid preprocessing directive #pragma " + std::string(l.token_data, l.token_size));  // gcc
        l.PreprocessorLeave();
        return;
    }
    if (l.IfToken("define"))
        return PreprocessorDefine();
    if (l.IfToken("undef"))
        return PreprocessorUndef();
    if (l.IfToken("else"))
        return PreprocessorElse();
    if (l.IfToken("endif"))
        return PreprocessorEndif();
    if (l.IfToken("ifdef"))
        return PreprocessorIfdef();
    if (l.IfToken("ifndef"))
        return PreprocessorIfndef();
    // TODO: #if #elif #elifdef #elifndef #line #embed #error #warning
    l.Error("invalid preprocessing directive #" + std::string(l.token_data, l.token_size));  // gcc
}

void CParserFile::PreprocessorInclude() {
    const char *arg = l.token_data;
    size_t arg_size = strlen(arg);

    bool current_dir;
    if (arg_size >= 2 && arg[0] == '"' && arg[arg_size - 1] == '"') {
        current_dir = true;
    } else if (arg_size >= 2 && arg[0] == '<' && arg[arg_size - 1] == '>') {
        current_dir = false;
    } else {
        l.Error("#include expects \"FILENAME\" or <FILENAME>");  // gcc
        return;
    }

    std::string file_name(arg + 1, arg_size - 2);
    l.PreprocessorLeave();

    std::string full_file_name;
    if (current_dir) {
        if (!cparser.FindAnyIncludeFile(file_name, l.file_name, full_file_name)) {
            l.Error("file \"" + file_name + "\" not found, local path \"" + l.file_name + "\"");
            return;
        }
    } else {
        if (!cparser.FindGlobalIncludeFile(file_name, full_file_name)) {
            l.Error("file \"" + file_name + "\" not found");
            return;
        }
    }

    const char *cached_file_name = nullptr;
    const char *contents = cparser.LoadFile(full_file_name, &cached_file_name);
    l.Include(contents, cached_file_name);
}

void CParserFile::PreprocessorPragmaCodepage() {
    if (!l.WantToken("("))
        return;
    CErrorPosition p1(l);
    std::string from_str;
    if (!l.WantString1(from_str))
        return;
    uint32_t from = 0;
    const char *f = from_str.c_str();
    const char *f_end = f + from_str.size();
    if (!ParseUtf8(from, f, f_end) || f != f_end)
        return programm.Error(p1, "incorrect char");
    if (!l.WantToken(","))
        return;
    CErrorPosition p2(l);
    uint64_t to = l.token_integer;
    if (!l.WantToken(CT_INTEGER))
        return;
    if (to < 0 || to > UINT8_MAX)
        return programm.Error(p2, "incorrect number");
    if (!l.WantToken(")"))
        return;
    if (!l.WantToken(CT_EOF))
        return;
    codepage[from] = to;
}

void CParserFile::PreprocessorPragmaOnce() {
    if (!l.WantToken(CT_EOF))
        return;
    if (!pragma_once.try_emplace(l.file_name, 0).second)
        l.PreprocessorSkipFile();
}

void CParserFile::PreprocessorDefine() {
    bool args_e = (l.cursor[0] == '(');
    std::string id;
    if (!l.WantIdent(id))
        return;

    std::vector<std::string> args;
    if (args_e) {
        if (!l.WantToken("("))
            return;
        do {
            std::string id;
            if (!l.WantIdent(id))
                return;
            args.push_back(id);
        } while (l.IfToken(","));
        if (!l.WantToken(")"))
            return;
    }

    l.PreprocessorLeave();
    l.AddMacro(id, l.token_data, strlen(l.token_data), &args);
}

void CParserFile::PreprocessorIfdef() {
    std::string id;
    if (!l.WantIdent(id))
        return;
    if (!l.WantToken(CT_EOF))
        return;

    l.PreprocessorIf(l.FindMacro(id));
}

void CParserFile::PreprocessorIfndef() {
    std::string id;
    if (!l.WantIdent(id))
        return;
    if (!l.WantToken(CT_EOF))
        return;

    l.PreprocessorIf(!l.FindMacro(id));
}

void CParserFile::PreprocessorUndef() {
    std::string id;
    if (!l.WantIdent(id))
        return;
    if (!l.WantToken(CT_EOF))
        return;

    l.PreprocessorLeave();
    l.DeleteMacro(id);
}

void CParserFile::PreprocessorElse() {
    if (!l.WantToken(CT_EOF))
        return;
    if (!l.PreprocessorElse())
        l.Error("#else without #if");  // gcc
}

void CParserFile::PreprocessorEndif() {
    if (!l.WantToken(CT_EOF))
        return;
    if (!l.PreprocessorEndIf())
        l.Error("#endif without #if");  // gcc
}
