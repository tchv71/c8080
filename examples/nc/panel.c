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
#include <c8080/tolowercase.h>
#include "colors.h"
#include "nc.h"
#include "tools.h"

size_t max_panel_files;
struct Panel panel_a;
struct Panel panel_b;
bool panels_hidden;
uint8_t panel_x;

uint16_t PanelGetCursorIndex(void) {
    return panel_a.offset + panel_a.cursor_y + panel_a.cursor_x * PANEL_ROWS_COUNT;
}

struct FileInfo *PanelGetCursor(void) {
    return panel_a.files + PanelGetCursorIndex();
}

uint8_t PanelGetDrive(void) {
    return panel_a.drive_user & 0x0F;
}

uint8_t PanelGetDirIndex(void) {
    return panel_a.drive_user >> 4;
}

static void PanelDrawTop(uint8_t x) {
    DrawTextXY(x, 0, COLOR_PANEL_BORDER, "╔══════════════════════════════╗");
}

void PanelDrawBorder(uint8_t x) {
    PanelDrawTop(x);
    for (uint8_t y = 1; y < TEXT_HEIGHT - 6; y++)
        DrawTextXY(x, y, COLOR_PANEL_BORDER, "║              │               ║");
    DrawTextXY(x + 6, 1, COLOR_PANEL_HEADER, "Name");
    DrawTextXY(x + 6 + 15, 1, COLOR_PANEL_HEADER, "Name");
    DrawTextXY(x, TEXT_HEIGHT - 6, COLOR_PANEL_BORDER, "╟──────────────┴───────────────╢");
    DrawTextXY(x, TEXT_HEIGHT - 5, COLOR_PANEL_BORDER, "║                              ║");
    DrawTextXY(x, TEXT_HEIGHT - 4, COLOR_PANEL_BORDER, "║                              ║");
    DrawTextXY(x, TEXT_HEIGHT - 3, COLOR_PANEL_BORDER, "╚══════════════════════════════╝");
}

void PanelDrawTitle(uint8_t color) {
    PanelDrawTop(panel_x);
    DrawTextXY(panel_x + (PANEL_WIDTH - strlen(panel_a.short_path)) / 2, 0, color, panel_a.short_path);
}

void PanelDrawFreeSpace(void) {
    if (panel_a.total_bytes != 0) {
        static char text[] = "           kbytes free on drive A";
        Uint32ToString(text, panel_a.free_bytes, 10);
        text[10] = ' ';
        text[sizeof(text) - 2] = 'A' + PanelGetDrive();
        DrawTextXY(panel_x + 2, TEXT_HEIGHT - 4, COLOR_PANEL_FOOTER, text + 5);
    }
}

static void DrawPanelFileInfoInt(const char *text) {
    DrawTextXY(panel_x + 2, TEXT_HEIGHT - 5, COLOR_PANEL_FOOTER, text);
}

void PanelDrawFileInfo(void) {
    if (panel_a.count == 0) {
        DrawPanelFileInfoInt(SPACES(TEXT_WIDTH / 2 - 4));
        return;
    }
    struct FileInfo *file_pointer = PanelGetCursor();
    CpmConvertFromName83(panel_a.selected_name, file_pointer->name83);
    char text[29];
    memset(text, ' ', sizeof(text) - 1);
    memcpy(text, panel_a.selected_name, strlen(panel_a.selected_name));
    if (file_pointer->attrib & ATTRIB_DIR) {
        strcpy(&text[12 + 7], file_pointer->name83[0] == '.' ? "►UP--DIR◄" : "►SUB-DIR◄");
    } else {
        Uint32ToString(text + 12, (uint32_t)file_pointer->blocks_128 << 7, 10);
        strcpy(&text[12 + 10], " bytes");
    }
    DrawPanelFileInfoInt(text);
}

static void DrawPanelFileInt(uint8_t *tile, struct FileInfo *file_info, uint8_t color) {
    static char screen_text[13] = "            ";
    memcpy(screen_text, file_info->name83, 8);
    memcpy(screen_text + 9, file_info->name83 + 8, 3);

    if ((file_info->attrib & ATTRIB_DIR) == 0)
        ToLowerCase(screen_text);

    if (color == 0)
        color = (file_info->attrib & ATTRIB_DIR) ? COLOR_PANEL_DIR : COLOR_PANEL_FILE;

    DrawText(tile, 0, color, screen_text);
}

void PanelDrawCursor(uint8_t color) {
    if (panel_a.count != 0)
        DrawPanelFileInt(TILE(PANEL_OX + panel_a.cursor_x * PANEL_COLUMN_WIDTH + panel_x, PANEL_OY + panel_a.cursor_y),
                         PanelGetCursor(), color);
}

void PanelHideCursor(void) {
    PanelDrawCursor(0);
}

void PanelShowCursor(void) {
    PanelDrawCursor(COLOR_PANEL_CURSOR);
    PanelDrawFileInfo();
}

void PanelDrawFiles(void) {
    size_t file_index = panel_a.offset;
    for (uint8_t x = 0; x < PANEL_COLUMNS_COUNT; x++) {
        uint8_t *screen_pointer = TILE(PANEL_OX + x * PANEL_COLUMN_WIDTH + panel_x, PANEL_OY);
        struct FileInfo *file_pointer = panel_a.files + file_index;
        for (uint8_t y = 0; y != PANEL_ROWS_COUNT; y++, screen_pointer += 64) {
            if (file_index >= panel_a.count) {
                DrawText(screen_pointer, 0, COLOR_PANEL_FILE, SPACES(12));
                continue;
            }
            DrawPanelFileInt(screen_pointer, file_pointer, 0);
            file_pointer++;
            file_index++;
        }
    }
}

void PanelMoveCursorLeft(void) {
    PanelHideCursor();
    if (panel_a.cursor_x != 0) {
        panel_a.cursor_x--;
    } else if (panel_a.offset != 0) {
        if (panel_a.offset > PANEL_ROWS_COUNT)
            panel_a.offset -= PANEL_ROWS_COUNT;
        else
            panel_a.offset = 0;
        PanelDrawFiles();
    } else if (panel_a.cursor_y != 0) {
        panel_a.cursor_y = 0;
    }
    PanelShowCursor();
}

void PanelMoveCursorRight(void) {
    PanelHideCursor();

    // Переместится вправо нельзя
    uint16_t w = PanelGetCursorIndex();
    if (w + PANEL_ROWS_COUNT >= panel_a.count) {  //! перепутаны > и >=
        // Это последний файл
        if (w + 1 >= panel_a.count) {
            PanelShowCursor();
            return;
        }
        // Вычисляем положение по Y
        panel_a.cursor_y = panel_a.count - (panel_a.offset + panel_a.cursor_x * PANEL_ROWS_COUNT + 1);
        // Корректируем курсор
        if (panel_a.cursor_y > PANEL_ROWS_COUNT - 1) {
            panel_a.cursor_y -= PANEL_ROWS_COUNT;
            if (panel_a.cursor_x == 1) {
                panel_a.offset += PANEL_ROWS_COUNT;
                PanelDrawFiles();
            } else {
                panel_a.cursor_x++;
            }
        }
    } else if (panel_a.cursor_x == 1) {
        panel_a.offset += PANEL_ROWS_COUNT;
        PanelDrawFiles();
    } else {
        panel_a.cursor_x++;
    }
    PanelShowCursor();
}

void PanelMoveCursorUp(void) {
    PanelHideCursor();
    if (panel_a.cursor_y != 0) {
        panel_a.cursor_y--;
    } else if (panel_a.cursor_x != 0) {
        panel_a.cursor_x--;
        panel_a.cursor_y = PANEL_ROWS_COUNT - 1;
    } else if (panel_a.offset != 0) {
        panel_a.offset--;
        PanelDrawFiles();
    }
    PanelShowCursor();
}

void PanelMoveCursorDown(void) {
    if (PanelGetCursorIndex() + 1 >= panel_a.count)
        return;
    PanelHideCursor();
    if (panel_a.cursor_y < PANEL_ROWS_COUNT - 1) {
        panel_a.cursor_y++;
    } else if (panel_a.cursor_x == 0) {
        panel_a.cursor_y = 0;
        panel_a.cursor_x++;
    } else {
        panel_a.offset++;
        PanelDrawFiles();
    }
    PanelShowCursor();
}

void PanelSwap(void) {
    panel_x = TEXT_WIDTH / 2 - panel_x;
    memswap(&panel_a, &panel_b, sizeof(panel_a));
}
