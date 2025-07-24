/*
 * c8080 stdlib
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

#include <codepage/koi7.h>

void ClearScreen(void) __link("86rk/clearscreen.c");
void MoveCursorHome(void) __link("86rk/movecursorhome.c");
void ShowCursor(void) __link("86rk/showcursor.c");
void HideCursor(void) __link("86rk/hidecursor.c");
uint8_t ScanKey(void) __address(0xF81B);

static const uint8_t KEY_UP = 0x19;
static const uint8_t KEY_LEFT = 0x08;
static const uint8_t KEY_RIGHT = 0x18;
static const uint8_t KEY_DOWN = 0x1A;
static const uint8_t KEY_ENTER = 0x0D;
static const uint8_t KEY_BACKSPACE = 0x7F;

static const unsigned TEXT_WIDTH = 64;
