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

#include "windows.h"
#include <c8080/hal.h>
#include <c8080/keys.h>
#include <string.h>
#include "colors.h"
#include "tools.h"
#include "stdio.h"
#include "nc.h"

char input[128];
uint8_t input_pos;
uint8_t window_color = COLOR_WINDOW;

static void MakeString(char *str, char c, uint8_t l) {
    memset(str, c, l);
    str[l] = 0;
}

void DrawWindowTextCenter(uint8_t y, const char *text) {
    DrawTextXY((TEXT_WIDTH - strlen(text)) / 2, y, window_color, text);
}

void DrawWindowText(uint8_t y, const char *text) {
    DrawTextXY(WINDOW_X, y, window_color, text);
}

uint8_t DrawWindow(uint8_t height, const char *title) {
    const uint8_t y0 = ((TEXT_HEIGHT - 4) - height) / 2;
    uint8_t y = y0;
    DrawWindowTextCenter(y, "                                      ");
    y++;
    DrawWindowTextCenter(y, "  ╔════════════════════════════════╗  ");
    DrawWindowTextCenter(y, title);
    y++;
    do {
        DrawWindowTextCenter(y, "  ║                                ║  ");
        y++;
        height--;
    } while (height != 0);
    DrawWindowTextCenter(y, "  ╚════════════════════════════════╝  ");
    y++;
    DrawWindowTextCenter(y, "                                      ");
    return y0 + 3;
}

void DrawInput(uint8_t x, uint8_t y, uint8_t width, uint8_t color) {
    const uint8_t input_pos_1 = input_pos + 1;
    uint8_t offset = 0;
    if (input_pos_1 > width)
        offset = input_pos_1 - width;
    DrawTextXY(x, y, color, input + offset);
    DrawTextXY(x + (input_pos - offset), y, color, "_");
    if (input_pos_1 < width)
        DrawTextXY(x + input_pos_1, y, color, SPACES(width + offset - input_pos_1));
}

void ProcessInput(char c) {
    if (c == KEY_BACKSPACE) {
        if (input_pos > 0)
            input_pos--;
    } else if (c >= ' ') {
        if (input_pos < sizeof(input) - 2) {
            input[input_pos] = c;
            input_pos++;
        }
    }
    input[input_pos] = 0;
}

bool RunInput(uint8_t y) {
    for (;;) {
        DrawInput(WINDOW_X, y, WINDOW_WIDTH, COLOR_INPUT);
        const char c = getchar();
        if (c == KEY_ENTER) {
            input_pos = 0;
            NcDrawScreen();
            return true;
        }
        if (c == KEY_ESC) {
            input_pos = 0;
            input[input_pos] = 0;
            NcDrawScreen();
            return false;
        }
        ProcessInput(c);
    }
}

void DrawProgress(uint8_t y) {
    char buf[PROGRESS_WIDTH + 1];
    MakeString(buf, '░', PROGRESS_WIDTH);
    DrawWindowText(y, buf);
}

void DrawProgressNext(uint8_t y, uint8_t width) {
    char buf[PROGRESS_WIDTH + 1];
    if (width > PROGRESS_WIDTH)
        width = PROGRESS_WIDTH;
    MakeString(buf, '█', width);
    DrawWindowText(y, buf);
}

static uint8_t DrawButton(uint8_t x, uint8_t y, uint8_t active, const char *text, uint8_t text_size) {
    char buf[16];
    MakeString(buf, ' ', text_size + 4);
    memcpy(buf + 2, text, text_size);
    DrawTextXY(x, y, active ? COLOR_BUTTON_ACTIVE : COLOR_BUTTON, buf);
    text_size += 4;
    DrawTextXY(x + text_size, y, CHANGE_INK_TO_BLACK(window_color), "▄");
    memset(buf, '▀', text_size);
    DrawTextXY(x + 1, y + 1, CHANGE_INK_TO_BLACK(window_color), buf);
}

uint8_t DrawButtons(uint8_t y, uint8_t cursor, const char *items) {
    uint8_t i;
    uint8_t x = -3;
    uint8_t step = 0;
    do {
        i = 0;
        const char *p = items;
        do {
            uint8_t l = strlen(p);
            if (step == 1)
                DrawButton(x, y, cursor == i, p, l);
            i++;
            x += l + (4 + 3);
            p += l + 1;
        } while (*p);
        x = (TEXT_WIDTH - x) / 2;
        step++;
    } while (step < 2);
    return i;
}

uint8_t RunButtons(uint8_t y, uint8_t cursor, const char *items) {
    for (;;) {
        const uint8_t count = DrawButtons(y, cursor, items);
        switch (getchar()) {
            case KEY_LEFT:
                cursor--;
                if (cursor > count)
                    cursor = count - 1;
                break;
            case KEY_RIGHT:
                cursor++;
                if (cursor >= count)
                    cursor = 0;
                break;
            case KEY_ENTER:
                NcDrawScreen();
                return cursor;
            case KEY_ESC:
                NcDrawScreen();
                return -1;
        }
    }
}

bool DeleteWindow(const char *file_name) {
    const uint8_t y = DrawWindow(6, " Delete ");
    DrawWindowTextCenter(y, "Do you wish to delete");
    DrawWindowTextCenter(y + 1, file_name);
    return RunButtons(y + 3, 0, "Yes\0No\0") == 0;
}

void ErrorWindow(const char *text) {
    window_color = COLOR_ERROR_WINDOW;
    const uint8_t y = DrawWindow(5, " Error ");
    DrawWindowTextCenter(y, text);
    RunButtons(y + 2, 0, "Ok\0");
    window_color = COLOR_WINDOW;
}

uint8_t SelectDriveWindow(uint8_t cursor, bool is_right) {
    const uint8_t y = DrawWindow(5, " Drive letter ");
    DrawWindowTextCenter(y, is_right ? "Choose right drive:" : "Choose left drive:");
    return RunButtons(y + 2, cursor, "A\0B\0C\0D\0");  // TODO: Config
}
