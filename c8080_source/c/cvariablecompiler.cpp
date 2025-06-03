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

#include "cvariablecompiler.h"
#include "cvariable.h"

void CVariableCompiler::AddCalledBy(const std::shared_ptr<CVariable> &function, bool call) {
    for (auto &i : called_by) {  // TODO: Optimize, change to map
        if (i.function.lock() == function) {
            if (call)
                i.call = true;
            return;
        }
    }
    called_by.push_back(CalledBy{function, call});
    function->c.call_count++;
}
