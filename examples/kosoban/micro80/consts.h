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

static const uint8_t CELL_W = 4;
static const uint8_t CELL_H = 3;

static const uint8_t GAME_X = 4;
static const uint8_t GAME_Y = 1;

static const uint8_t INTRO_Y = 24;

static const uint8_t LEVEL_X = (TEXT_WIDTH - 10) / 2;  // "Уровень 00"
static const uint8_t LEVEL_Y = 0;
static const uint8_t LEVEL_COLOR = COLOR_INK_LIGHT_WHITE | COLOR_PAPER_BLUE;

static const uint8_t MOVE_ANIMATION_H = 1;

static const uint8_t COLOR_LEVEL = COLOR_INK_YELLOW;
static const uint8_t INTRO_COLOR = COLOR_INK_LIGHT_CYAN;

#define BOX_COLOR(X) (X)
#define DEST_COLOR(X) (X)
