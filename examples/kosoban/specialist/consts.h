/*
 * Game "Kosoban" for 8 bit computers
 * Programming (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 * Levels and idea (c) Andrey Shevchuk campugnatus@gmail.com
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
#include <c8080/colors.h>

static const uint8_t CELL_W = imgCellWidth;
static const uint8_t CELL_H = imgCellHeight;

static const uint8_t GAME_X = 3;
static const uint8_t GAME_Y = 48;

static const uint8_t INTRO_Y = 248;

static const uint8_t MOVE_ANIMATION_H = 8;

static const uint8_t LEVEL_X = 128;
static const uint8_t LEVEL_Y = 24;

static const uint8_t COLOR_LEVEL = COLOR_INK_YELLOW;
static const uint8_t COLOR_INTRO = COLOR_INK_CYAN;

#pragma codepage('©', 0x2D)

static const uint8_t allColors[] = {
    COLOR_INK_WHITE,
    COLOR_INK_RED,
    COLOR_INK_GREEN,
    COLOR_INK_BLUE,
    COLOR_INK_YELLOW,
    COLOR_INK_CYAN,
    COLOR_INK_VIOLET,
    COLOR_INK_WHITE,
};

#define BOX_COLOR(X) (allColors[(X)])
#define DEST_COLOR(X) (allColors[(X)])
