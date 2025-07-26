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
#include "common.h"

namespace I8080 {

static bool DeadCode(AsmBase &a) {
    bool label_removed = false, dead_code = true;
    for (auto &l : a.lines) {
        if (dead_code && l.opcode != AC_LABEL) {
            if ((l.opcode == AC_JMP || l.opcode == AC_JMP_CONDITION) && l.argument[0].label)
                if (UnrefLabel(a, l.argument[0].label))
                    label_removed = true;
            l.opcode = AC_REMOVED;
        } else {
            dead_code = (l.opcode == AC_JMP || l.opcode == AC_RET);
        }
    }
    return label_removed;
}

void AsmOptimizeDeadCode(AsmBase &a) {
    while (DeadCode(a)) {
    }
}

}  // namespace I8080
