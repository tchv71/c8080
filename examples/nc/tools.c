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

extern uint8_t bios_exec_mode __address(0xF700);
extern uint8_t cpm_current_drive_user __address(0xF701);
extern uint8_t storage_state __address(0xF702);
extern uint8_t storage_drive_user_b __address(0xF703);

static const uint8_t STATE_TAB = 1 << 4;
static const uint8_t STATE_HIDDEN = 1 << 5;

void SaveScreen(void) {
    bios_exec_mode = 1;

    // Что бы командер нижней строкой не закрывал полезные данные
    const uint16_t xy = GetCursorPosition();
    if (xy >= (TEXT_HEIGHT - 1) << 8) {
        CpmConsoleWrite('\n');
        MoveCursor(xy, (xy >> 8) - 1);
    }

    // Скрываем курсор
    HideCursor();

    // Сохраняем экран
    memcpy(saved_screen, (void *)0xE000, TEXT_WIDTH * TEXT_HEIGHT);
    memcpy(saved_screen + TEXT_WIDTH * TEXT_HEIGHT, (void *)0xE800, TEXT_WIDTH * TEXT_HEIGHT);

    // Восстанавливаем состояние
    panelA.drive_user = CpmGetCurrentDrive() | (cpm_current_drive_user & 0xF0);

    panelB.drive_user = storage_drive_user_b;
    if ((panelB.drive_user & 0x0F) >= DRIVE_COUNT)
        panelB.drive_user = panelA.drive_user;

    hidden = (storage_state & STATE_HIDDEN) != 0;
    if (storage_state & STATE_TAB) {
        if (videoOffset != 0)
            videoOffset = 0;
        else
            videoOffset = TEXT_WIDTH / 2;
    }
}

void RestoreScreen(void) {
    memcpy((void *)0xE000, saved_screen, TEXT_WIDTH * TEXT_HEIGHT);
    memcpy((void *)0xE800, saved_screen + TEXT_WIDTH * TEXT_HEIGHT, TEXT_WIDTH * TEXT_HEIGHT);
}

void ExitScreen(void) {
    // Сохраняем состояние
    storage_drive_user_b = panelB.drive_user;
    if (videoOffset)
        storage_state |= STATE_TAB;
    if (hidden)
        storage_state |= STATE_HIDDEN;

    // Восстанавливаем экран
    RestoreScreen();

    // Показываем курсор
    ShowCursor();
}

void NcDisableAutorun(void) {
    bios_exec_mode = 0;
}
