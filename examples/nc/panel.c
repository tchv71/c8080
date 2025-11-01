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

#include "panel.h"
#include <c8080/hal.h>
#include <c8080/uint32tostring.h>
#include <string.h>
#include "colors.h"
#include "nc.h"
#include "tools.h"

char selected_file_name[13];

uint8_t GetPanelDrive(void) {
    return panelA.drive_user & 0x0F;
}

uint8_t GetPanelUser(void) {
    return panelA.drive_user >> 4;
}

void DrawPanel(uint8_t x) {
    DrawTextXY(x, 0, COLOR_PANEL_BORDER, "╔══════════════════════════════╗");
    uint8_t y;
    for (y = 1; y < TEXT_HEIGHT - 6; y++)
        DrawTextXY(x, y, COLOR_PANEL_BORDER, "║              │               ║");
    DrawTextXY(x + 6, 1, COLOR_PANEL_HEADER, "Name");
    DrawTextXY(x + 6 + 15, 1, COLOR_PANEL_HEADER, "Name");
    DrawTextXY(x, TEXT_HEIGHT - 6, COLOR_PANEL_BORDER, "╟──────────────┴───────────────╢");
    DrawTextXY(x, TEXT_HEIGHT - 5, COLOR_PANEL_BORDER, "║                              ║");
    DrawTextXY(x, TEXT_HEIGHT - 4, COLOR_PANEL_BORDER, "║                              ║");
    DrawTextXY(x, TEXT_HEIGHT - 3, COLOR_PANEL_BORDER, "╚══════════════════════════════╝");
}

void DrawPanelTitle(uint8_t color) {
    DrawTextXY(videoOffset + (TEXT_WIDTH / 2 - strlen(panelA.path)) / 2, 0, color, panelA.path);
}

void DrawPanelFreeSpace(void) {
    if (panelA.total_bytes != 0) {
        static char text[] = "           kbytes free on drive  ";
        Uint32ToString(text, panelA.free_bytes, 10);
        text[10] = ' ';
        text[sizeof(text) - 2] = 'A' + GetPanelDrive();
        DrawTextXY(videoOffset + 2, TEXT_HEIGHT - 4, COLOR_PANEL_FOOTER, text + 5);
    }
}

static void DrawPanelFileInfoInt(const char *text) {
    DrawTextXY(videoOffset + 2, TEXT_HEIGHT - 5, COLOR_PANEL_FOOTER, text);
}

void DrawPanelFileInfo(void) {
    if (panelA.count == 0) {
        DrawPanelFileInfoInt(SPACES(TEXT_WIDTH / 2 - 4));
        return;
    }
    struct FileInfo *file_pointer = CurrentFile();
    memcpy(panelA.select_name, file_pointer->name83, sizeof(panelA.select_name));
    CpmNormalizeName(selected_file_name, file_pointer->name83);
    char text[29];
    memset(text, ' ', sizeof(text) - 1);
    memcpy(text, selected_file_name, strlen(selected_file_name));
    if (file_pointer->attrib & ATTRIB_DIR) {
        strcpy(&text[12 + 7], file_pointer->name83[0] == '.' ? "►UP--DIR◄" : "►SUB-DIR◄");
    } else {
        Uint32ToString(text + 12, (uint32_t)file_pointer->blocks_128 << 7, 10);
        strcpy(&text[12 + 10], " bytes");
    }
    DrawPanelFileInfoInt(text);
}
