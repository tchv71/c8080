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

static const unsigned LEVEL_COUNT = 36;

static const uint8_t LEVEL_WIDTH = 5;
static const uint8_t LEVEL_HEIGHT = 7;
static const uint8_t LEVEL_PLANES = 2;
static const uint8_t LEVEL_SIZE = LEVEL_WIDTH * LEVEL_HEIGHT * LEVEL_PLANES;

static const uint8_t DIR_SHIFT = 4;
static const uint8_t DIR_UP = 1 << DIR_SHIFT;
static const uint8_t DIR_RIGHT = 2 << DIR_SHIFT;
static const uint8_t DIR_DOWN = 3 << DIR_SHIFT;
static const uint8_t DIR_LEFT = 4 << DIR_SHIFT;
static const uint8_t DIR_MASK = 0xFF << DIR_SHIFT;

#define GET_DIR(X) ((X)&DIR_MASK)
#define GET_COLOR(X) ((X) & ~DIR_MASK)

static const uint8_t EMPTY = 0;

extern const uint8_t levels[LEVEL_COUNT * LEVEL_SIZE];
