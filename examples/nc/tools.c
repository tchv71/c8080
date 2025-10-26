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
#include "tools.h"

const char spaces[TEXT_WIDTH + 1] = "                                                                ";

static char saved_screen[64 * 25 * 2];

void SaveScreen(void) {
    memcpy(saved_screen, (void *)0xE000, 64 * 25);
    memcpy(saved_screen + 64 * 25, (void *)0xE800, 64 * 25);
}

void RestoreScreen(void) {
    memcpy((void *)0xE000, saved_screen, 64 * 25);
    memcpy((void *)0xE800, saved_screen + 64 * 25, 64 * 25);
}
