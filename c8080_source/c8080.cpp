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

#include <iostream>
#include <assert.h>
#include "tools/catpath.h"
#include "tools/getpath.h"
#include "tools/direxists.h"
#include "tools/fileexists.h"
#include "tools/removeextension.h"
#include "8080/Compile.h"
#include "c/tools/dump.h"
#include "c/parser/cparser.h"
#include "8080/cmm/prepare.h"
#include "prepare/prepare.h"
#include "8080/cmm/cmm.h"
#include "8080/cmm/names.h"

#ifdef __MINGW32__
static const char *ASSEMBLER = "sjasmplus.exe";
#else
static const char *ASSEMBLER = "sjasmplus";
#endif

struct Options {
    I8080::OutputFormat output_format = I8080::OF_CPM;  // -O
    std::string bin_file_name;                          // -o
    std::string asm_file_name;                          // -a
    bool print_expression_tree = false;                 // -V
    bool assembler_need_path = true;                    // -A
    std::string assembler = ASSEMBLER;                  // -A
};

static void Usage(char **argv) {
    std::cout << "Usage: " << argv[0] << " [options] file1.c file2.c ..." << std::endl
              << "Options:" << std::endl
              << "  -m         Compile CMM language" << std::endl
              << "  -I<path>   Add include directory" << std::endl
              << "  -OCPM      Make binary file for CP/M" << std::endl
              << "  -Oi1080    Make binary file for Iskra 1080 Tartu" << std::endl
              << "  -D<define> Set #define" << std::endl
              << "  -o<file>   Set name for output binary file" << std::endl
              << "  -a<file>   Set name for output assembler file" << std::endl
              << "  -A<file>   Alternative assembler tool" << std::endl
              << "  -V         Print expression tree" << std::endl
              << "  --         Last option" << std::endl;
}

static void ParseOptions(int argc, char **argv, Options &o, CParser &c) {
    bool disable_options = false;
    for (int i = 1; i < argc; i++) {
        char *s = argv[i];
        if (s[0] == '-' && !disable_options) {
            if (s[2] == 0) {
                switch (s[1]) {
                    case 'V':
                        o.print_expression_tree = true;
                        continue;
                    case '-':
                        disable_options = true;
                        continue;
                    case 'm':
                        c.programm.cmm = true;
                        continue;
                }
            }
            const char *value = s + 2;
            if (value[0] == 0) {
                if (i + 1 >= argc)
                    throw std::runtime_error(std::string("missing value after '") + s + "'"); // gcc
                i++;
                value = argv[i];
            }
            switch (s[1]) {
                case 'I':
                    c.include_dirs.push_back(value);
                    continue;
                case 'O':
                    if (I8080::ParseOutputFormat(o.output_format, value))
                        continue;
                    break;
                case 'D':
                    c.default_defines.push_back(value);  // TODO: NAME=VALUE
                    continue;
                case 'o':
                    o.bin_file_name = value;
                    continue;
                case 'a':
                    o.asm_file_name = value;
                    continue;
                case 'A':
                    o.assembler = value;
                    o.assembler_need_path = false;
                    continue;
            }
            throw std::runtime_error("unrecognized command-line option '" + std::string(s) + "'");  // gcc
        }
        c.AddSourceFile(s);
    }
}

#ifdef WIN32
int _tmain(int argc, _TCHAR *argv[]) {
    setlocale(LC_ALL, "RUSSIAN");
#else
int main(int argc, char **argv) {
#endif
    try {
        std::cout << "C Compiler for i8080 (" __DATE__ << ")" << std::endl
                  << "(c) Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru" << std::endl;

        if (argc < 2) {
            Usage(argv);
            return 0;
        }

        CProgramm programm;
        CParser c(programm);

        std::string std_include_dir = CatPath(GetPath(argv[0]), "include");
        if (DirExists(std_include_dir))
            c.include_dirs.push_back(std_include_dir);

        Options o;
        ParseOptions(argc, argv, o, c);

        if (o.bin_file_name.empty() || o.asm_file_name.empty()) {
            std::string base_name;
            if (!c.GetFirstSourceFile(base_name))
                base_name = "a";
            RemoveExtension(base_name);

            if (o.bin_file_name.empty())
                o.bin_file_name = base_name + ".bin";
            if (o.asm_file_name.empty())
                o.asm_file_name = base_name + ".asm";
        }

        if (o.assembler_need_path) {
            std::string assembler1 = CatPath(GetPath(argv[0]), o.assembler);
            if (FileExists(assembler1)) {
                o.assembler = assembler1;
            } else if (!c.include_dirs.empty()) {
                o.assembler = CatPath(c.include_dirs[0], o.assembler);
            }
        }

        if (programm.cmm) {
            I8080::RegisterInternalCmmNames(programm);
            c.default_defines.push_back("__CMM");
        } else {
            I8080::RegisterInternalNames(programm);

            std::string internal_c_file_name;
            if (!c.FindGlobalIncludeFile("c8080/internal.c", internal_c_file_name))
                throw std::runtime_error("file \"c8080/internal.c\" not found");
            c.AddSourceFile(internal_c_file_name);
        }

        c.ParseAll();

        if (programm.error)
            return 1;

        if (programm.cmm) {
            I8080::CompileCmm(programm, o.asm_file_name);
        } else {
            if (o.print_expression_tree)
                Dump(programm.first_node, "");

            I8080::Compile(c, programm, o.output_format, o.bin_file_name, o.asm_file_name);
        }

        if (programm.error)
            return 1;

        std::string asm_cmd_line = o.assembler + " --lst " + o.asm_file_name;
        int r = system(asm_cmd_line.c_str());
        if (r != 0)
            throw std::runtime_error("Assembler error (" + asm_cmd_line + ")");

//        system("./go");  // TODO: Remove

        std::cout << "Done" << std::endl;
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
