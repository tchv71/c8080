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

#include <stdint.h>
#include <stddef.h>
#include <c8080/hal.h>

static const uint8_t PANEL_OX = 2;
static const uint8_t PANEL_OY = 2;
static const uint8_t PANEL_COLUMN_WIDTH = 15;
static const uint8_t PANEL_COLUMNS_COUNT = 2;
static const uint8_t PANEL_ROWS_COUNT = TEXT_HEIGHT - 8;
static const uint8_t PANEL_WIDTH = TEXT_WIDTH / 2;

struct FileInfo {
    char name83[8 + 3];
    uint8_t attrib;
    uint16_t blocks_128;
};

static const uint8_t ATTRIB_DIR = 0x80;
static const uint8_t ATTRIB_DIR_MAX = 0x0F;
static const uint8_t ATTRIB_DIR_SHIFT = 3;
static const uint8_t ATTRIB_DIR_MASK = ATTRIB_DIR_MAX << ATTRIB_DIR_SHIFT;
static const uint8_t ATTRIB_DIR_ALL = ATTRIB_DIR_MASK | ATTRIB_DIR;
#define GET_DIR_FROM_ATTRIB(X) (((X) >> ATTRIB_DIR_SHIFT) & ATTRIB_DIR_MAX)

struct Panel {
    uint8_t copy_start[0];
    uint8_t drive_user;
    char path[128];
    uint32_t total_bytes;
    uint32_t free_bytes;
    uint16_t count;
    uint8_t copy_end[0];

    char *short_path;
    uint16_t offset;
    uint8_t cursor_x;
    uint8_t cursor_y;
    struct FileInfo *files;
    char selected_name[8 + 1 + 3 + 1];
};

extern size_t max_panel_files;
extern struct Panel panel_a, panel_b;
extern uint8_t panel_x;

uint8_t PanelGetDrive(void);
uint8_t PanelGetDirIndex(void);
void PanelDrawBorder(uint8_t x);
void PanelDrawFreeSpace(void);
void PanelDrawFileInfo(void);
void PanelDrawTitle(uint8_t color);
void PanelDrawCursor(uint8_t color);
void PanelHideCursor(void);
void PanelShowCursor(void);
void PanelDrawFiles(void);
void PanelMoveCursorLeft(void);
void PanelMoveCursorRight(void);
void PanelMoveCursorUp(void);
void PanelMoveCursorDown(void);
uint16_t PanelGetCursorIndex(void);
struct FileInfo *PanelGetCursor(void);
void PanelSwap(void);
