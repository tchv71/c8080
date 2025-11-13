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

#include <string.h>
#include <c8080/console.h>
#include <cpm.h>
#include "tools.h"
#include "nc.h"
#include "config.h"

const char spaces[TEXT_WIDTH + 1] = "                                                                ";

static char saved_screen[TEXT_WIDTH * TEXT_HEIGHT * 2];
static uint16_t saved_cursor_position;

extern uint8_t glob_dont_exec __address(0xF740);
extern uint8_t glob_tdrive __address(0xF741);
extern uint8_t glob_state __address(0xF700);
extern uint8_t glob_drive_user_b __address(0xF701);
extern uint16_t glob_a_offset __address(0xF702);
extern uint8_t glob_a_cursor_x __address(0xF704);
extern uint8_t glob_a_cursor_y __address(0xF705);
extern uint16_t glob_b_offset __address(0xF706);
extern uint8_t glob_b_cursor_x __address(0xF708);
extern uint8_t glob_b_cursor_y __address(0xF709);

static const uint8_t STATE_TAB = 1 << 4;
static const uint8_t STATE_HIDDEN = 1 << 5;

void SaveScreen(void) {
    glob_dont_exec = 0;

    // Что бы командер нижней строкой не закрывал полезные данные
    const uint16_t xy = GetCursorPosition();
    if (xy >= (TEXT_HEIGHT - 1) << 8) {
        CpmConsoleWrite('\n');
        MoveCursor(xy, (xy >> 8) - 1);
    }

    // Скрываем курсор
    saved_cursor_position = GetCursorPosition();
    HideCursor();

    // Сохраняем экран
    memcpy(saved_screen, (void *)0xE000, TEXT_WIDTH * TEXT_HEIGHT);
    memcpy(saved_screen + TEXT_WIDTH * TEXT_HEIGHT, (void *)0xE800, TEXT_WIDTH * TEXT_HEIGHT);

    // Текущий диск и папку в активную панель
    panel_a.drive_user = CpmGetCurrentDrive() | (glob_tdrive & 0xF0);

    panel_a.cursor_x = glob_a_cursor_x;
    panel_a.cursor_y = glob_a_cursor_y;
    panel_a.offset = glob_a_offset;
    panel_b.cursor_x = glob_b_cursor_x;
    panel_b.cursor_y = glob_b_cursor_y;
    panel_b.offset = glob_b_offset;

    // Восстанавливаем состояние
    panel_b.drive_user = glob_drive_user_b;
    if ((panel_b.drive_user & 0x0F) >= DRIVE_COUNT)
        panel_b.drive_user = panel_a.drive_user;

    panels_hidden = (glob_state & STATE_HIDDEN) != 0;
    if (glob_state & STATE_TAB) {
        if (panel_x != 0)
            panel_x = 0;
        else
            panel_x = TEXT_WIDTH / 2;
    }
}

void RestoreConsole(void) {
    memcpy((void *)0xE000, saved_screen, TEXT_WIDTH * TEXT_HEIGHT);
    memcpy((void *)0xE800, saved_screen + TEXT_WIDTH * TEXT_HEIGHT, TEXT_WIDTH * TEXT_HEIGHT);
}

void ExitScreen(void) {
    // Сохраняем состояние
    glob_drive_user_b = panel_b.drive_user;
    glob_state = (panel_x ? STATE_TAB : 0);
    if (panels_hidden)
        glob_state |= STATE_HIDDEN;

    glob_a_cursor_x = panel_a.cursor_x;
    glob_a_cursor_y = panel_a.cursor_y;
    glob_a_offset = panel_a.offset;
    glob_b_cursor_x = panel_b.cursor_x;
    glob_b_cursor_y = panel_b.cursor_y;
    glob_b_offset = panel_b.offset;

    // Восстанавливаем экран
    RestoreConsole();
    MoveCursor(saved_cursor_position, saved_cursor_position >> 8);

    // Показываем курсор
    ShowCursor();
}

void NcDisableAutorun(void) {
    glob_dont_exec = 1;
}
