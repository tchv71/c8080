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

#include "common.h"

namespace I8080 {

bool UnrefLabel(AsmBase &a, AsmLabel *label) {
    assert(label != nullptr);

    label->used--;
    if (label->used != 1)
        return false;

    label->used--;
    AsmBase::Line &labelLine = a.lines[label->destination - 1];
    assert(labelLine.opcode == AC_LABEL);
    labelLine.opcode = AC_REMOVED;
    return true;
}

AsmLabel *GetLastLabel(AsmBase &a, AsmLabel *label) {
    std::vector<AsmLabel *> recursive;
    for (;;) {
        // Сломанная метка
        if (label == nullptr || label->destination >= a.lines.size())
            throw std::runtime_error("Incorrect label");
        // Куда указываем метка
        AsmBase::Line *destination_line = &a.lines[label->destination];

        while (destination_line->opcode == AC_REMOVED) {
            destination_line++;
            if (destination_line == &*a.lines.end())
                return label;
        }

        if (destination_line->opcode == AC_JMP && destination_line->argument[0].label) {
            // Обнаружена рекурсия
            if (std::find(recursive.begin(), recursive.end(), label) != recursive.end())
                break;
            recursive.push_back(label);
            label = destination_line->argument[0].label;
        }

        break;
    }
    return label;
}

AsmBase::Line *GetNextLine(AsmBase &a, AsmBase::Line *line) {
    if (line == nullptr)
        return nullptr;
    AsmBase::Line *const lines_end = &*a.lines.end();
    for (;;) {
        line++;
        if (line == lines_end)
            return nullptr;
        if (line->opcode != AC_REMOVED && line->opcode != AC_LINE && line->opcode != AC_REMARK)
            return line;
    }
}

AsmBase::Line *GetNextLineNoLabel(AsmBase &a, AsmBase::Line *line) {
    if (line == nullptr)
        return nullptr;
    AsmBase::Line *const lines_end = &*a.lines.end();
    for (;;) {
        line++;
        if (line == lines_end)
            return nullptr;
        if (line->opcode != AC_REMOVED && line->opcode != AC_LINE && line->opcode != AC_REMARK &&
            line->opcode != AC_LABEL)
            return line;
    }
}

AsmBase::Line *GetLineNoLabel(AsmBase &a, AsmBase::Line *line) {
    AsmBase::Line *const lines_end = &*a.lines.end();
    while (line + 1 < lines_end && (line->opcode == AC_REMOVED || line->opcode == AC_LINE ||
                                    line->opcode == AC_REMARK || line->opcode == AC_LABEL))
        line++;
    return line;
}

AsmBase::Line *GetCpuLineNoLabel(AsmBase::Line *line, AsmBase::Line *lines_end) {
    if (line == nullptr)
        return nullptr;
    for (;;) {
        line++;
        if (line == lines_end)
            return nullptr;
        if (line->opcode != AC_REMOVED && line->opcode != AC_LINE && line->opcode != AC_REMARK &&
            line->opcode != AC_LABEL)
            return line;
    }
}

};  // namespace I8080
