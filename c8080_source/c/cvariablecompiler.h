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

#pragma once

#include <stdint.h>
#include <string>
#include <memory>
#include <vector>

class CVariable;

struct CVariableCompiler {
    uint64_t function_stack_frame_offset{};
    bool equ_enabled{};
    std::string equ_text;  // Имя переменной в коде
    size_t use_counter{};
    std::shared_ptr<CVariable> static_stack;  // Переменная указывает на кадр стека функции

    struct CalledBy {
        std::weak_ptr<CVariable> function;
        bool call;
    };
    std::vector<CalledBy> called_by;  // Заполняется при расчете стека
    bool called_by_processed{};       // Заполняется при расчете стека
    size_t call_count{};              // Заполняется при расчете стека
    size_t internal_cmm_id{};         // Used when compiling CMM

    bool prepared{};

    void AddCalledBy(const std::shared_ptr<CVariable> &v, bool call);
};
