/*
 * c8080 compiler
 * Copyright (c) 2025 Aleksey Morozov
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

#include "index.h"

namespace I8080 {

void AsmOptimize(AsmBase &a) {
    AsmOptimizeJumpJump(a);
    AsmOptimizeDeadCode(a);
    AsmOptimizeLoadSave(a);
}

}  // namespace I8080
