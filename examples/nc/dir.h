/*
 * NC shell for CP/M
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

#include <cpm.h>
#include <stddef.h>

static const uint8_t MAX_DIRS = CPM_MAX_USERS - 1;

struct DirInfoItem {
    uint8_t parent;
    char name[8 + 1 + 3 + 1];
};

struct DirInfo {
    uint8_t drive;
    struct DirInfoItem items[MAX_DIRS];
};

void DirInfoReset(struct DirInfo *self, uint8_t drive);
uint8_t DirInfoAdd(struct DirInfo *self, struct FCB *fcb);
void DirInfoMakePath(struct DirInfo *self, char *out, uint8_t out_size, uint8_t dir_index);
uint8_t DirAllocate(void);
