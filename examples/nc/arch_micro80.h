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

#define NC_GLOB
#define NC_SAVE_SCREEN

// Переменные BIOS

extern uint8_t bios_dont_resart_nc __address(0xF740);
extern uint8_t bios_user __address(0xF741);

// Место в памяти Микро 80 специально выделенное для NC
// Оно сохраняется при теплой перезагрузке

extern uint8_t glob_state __address(0xF700);
extern uint8_t glob_drive_user_b __address(0xF701);
extern uint16_t glob_a_offset __address(0xF702);
extern uint8_t glob_a_cursor_x __address(0xF704);
extern uint8_t glob_a_cursor_y __address(0xF705);
extern uint16_t glob_b_offset __address(0xF706);
extern uint8_t glob_b_cursor_x __address(0xF708);
extern uint8_t glob_b_cursor_y __address(0xF709);
