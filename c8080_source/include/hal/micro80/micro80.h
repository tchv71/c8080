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
#include <codepage/micro80.h>

#define TILE(X, Y) (uint8_t *)(0xE800 + (X) + (Y)*64)

void ClearScreen(void) __link("clearscreen.c");
void ClearScreenBlack(void) __link("clearscreenblack.c");
void MoveCursorHome(void) __link("movecursorhome.c");
void ShowCursor(void) __link("showcursor.c");
void HideCursor(void) __link("hidecursor.c");
void SetConsoleColor(uint8_t color) __link("setconsolecolor.c");
void DrawImageTile(void *tile, const void *image, uint16_t width_height) __link("drawimagetile.c");
void DrawText(void *tile, uint8_t color, const char *text) __link("drawtext.c");
uint8_t ScanKey(void) __address(0xF81B);

// Color codes

static const uint8_t COLOR_UNDERLINE = 1 << 7;
static const uint8_t COLOR_INK_LIGHT = 1 << 6;

static const uint8_t COLOR_INK_BLACK = 0;
static const uint8_t COLOR_INK_GREEN = 2;
static const uint8_t COLOR_INK_RED = 1;
static const uint8_t COLOR_INK_BLUE = 4;
static const uint8_t COLOR_INK_CYAN = COLOR_INK_BLUE | COLOR_INK_GREEN;
static const uint8_t COLOR_INK_MAGENTA = COLOR_INK_BLUE | COLOR_INK_RED;
static const uint8_t COLOR_INK_YELLOW = COLOR_INK_RED | COLOR_INK_GREEN;
static const uint8_t COLOR_INK_WHITE = 7;
static const uint8_t COLOR_INK_GRAY = COLOR_INK_LIGHT | COLOR_INK_BLACK;
static const uint8_t COLOR_INK_LIGHT_BLUE = COLOR_INK_LIGHT | COLOR_INK_BLUE;
static const uint8_t COLOR_INK_LIGHT_RED = COLOR_INK_LIGHT | COLOR_INK_RED;
static const uint8_t COLOR_INK_LIGHT_CYAN = COLOR_INK_LIGHT | COLOR_INK_CYAN;
static const uint8_t COLOR_INK_LIGHT_GREEN = COLOR_INK_LIGHT | COLOR_INK_GREEN;
static const uint8_t COLOR_INK_LIGHT_MAGENTA = COLOR_INK_LIGHT | COLOR_INK_MAGENTA;
static const uint8_t COLOR_INK_LIGHT_YELLOW = COLOR_INK_LIGHT | COLOR_INK_YELLOW;
static const uint8_t COLOR_INK_LIGHT_WHITE = COLOR_INK_LIGHT | COLOR_INK_WHITE;

static const uint8_t COLOR_PAPER_BLACK = COLOR_INK_BLACK << 3;
static const uint8_t COLOR_PAPER_RED = COLOR_INK_RED << 3;
static const uint8_t COLOR_PAPER_BLUE = COLOR_INK_BLUE << 3;
static const uint8_t COLOR_PAPER_CYAN = COLOR_INK_CYAN << 3;
static const uint8_t COLOR_PAPER_GREEN = COLOR_INK_GREEN << 3;
static const uint8_t COLOR_PAPER_MAGENTA = COLOR_INK_MAGENTA << 3;
static const uint8_t COLOR_PAPER_YELLOW = COLOR_INK_YELLOW << 3;
static const uint8_t COLOR_PAPER_WHITE = COLOR_INK_WHITE << 3;

// Key codes

static const uint8_t KEY_UP = 0x19;
static const uint8_t KEY_LEFT = 0x08;
static const uint8_t KEY_RIGHT = 0x18;
static const uint8_t KEY_DOWN = 0x1A;
static const uint8_t KEY_ENTER = 0x0D;
static const uint8_t KEY_BACKSPACE = 0x7F;

// Other codes

static const uint8_t TEXT_WIDTH = 64;
static const uint8_t TEXT_HEIGHT = 25;
