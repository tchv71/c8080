/*
 * Game "Color Lines" for 8 bit computers
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
#include <c8080/colors.h>

static const uint8_t COLOR_STEP = 13;

static const uint8_t PLAYFIELD_Y = 1;
static const uint8_t PLAYFIELD_X = 18;
static const uint8_t CELL_TILE_WIDTH = 3;
static const uint8_t CELL_TILE_HEIGHT = 3;

static const uint8_t SCORE_COLOR = COLOR_INK_LIGHT_GREEN;
static const uint8_t SCORE_X = 53;
static const uint8_t SCORE_Y = 18;

static const uint8_t PLAYAR_MAX_HEIGHT = 3;
static const uint8_t PLAYER_Y = 16;
static const uint8_t PLAYER_X = 48;
static const uint8_t PLAYER_CROWN_DX = 5;
static const uint8_t PLAYER_CROWN_DY = 0;

static const uint8_t KING_X = 4;
static const uint8_t KING_Y = 1;

static const uint8_t HIRECT_X = 21;
static const uint8_t HIRECT_Y = 4;
static const uint8_t HIRECT_WIDTH = 5;
static const uint8_t HIRECT_HEIGHT = 5;

static const uint8_t HI_X = 24;
static const uint8_t HI_Y = 5;
static const uint8_t HI_LINE_HEIGHT = 1;
static const uint8_t HI_COLOR_0 = COLOR_INK_LIGHT_CYAN;
static const uint8_t HI_COLOR_1 = COLOR_INK_LIGHT_RED;
static const uint8_t HI_COLOR_2 = COLOR_INK_LIGHT_YELLOW;
static const uint8_t HI_COLOR_3 = COLOR_INK_LIGHT_WHITE;

static const uint8_t BUTTON_PATH_X = 5;
static const uint8_t BUTTON_PATH_Y = 20;
static const uint8_t BUTTON_PATH_W = 9;
static const uint8_t BUTTON_PATH_H = 1;
static const uint8_t BUTTON_SOUND_X = 5;
static const uint8_t BUTTON_SOUND_Y = 21;
static const uint8_t BUTTON_SOUND_W = 9;
static const uint8_t BUTTON_SOUND_H = 1;
static const uint8_t BUTTON_HELP_X = 5;
static const uint8_t BUTTON_HELP_Y = 22;
static const uint8_t BUTTON_HELP_W = 9;
static const uint8_t BUTTON_HELP_H = 1;
static const uint8_t BUTTON_COLOR_ENABLED = 0x70;
static const uint8_t BUTTON_COLOR_DISABLED = 0x78;

static const uint8_t HELP_1_X = 49;
static const uint8_t HELP_1_Y = 21;
static const uint8_t HELP_2_X = 53;
static const uint8_t HELP_2_Y = 21;
static const uint8_t HELP_3_X = 57;
static const uint8_t HELP_3_Y = 21;

static const uint8_t TOPSCORE_X = 7;
static const uint8_t TOPSCORE_Y = 18;
static const uint8_t TOPSCORE_COLOR = COLOR_INK_LIGHT_GREEN;
static const uint8_t TOPNAME_X = 0;
static const uint8_t TOPNAME_Y = 0;
static const uint8_t TOPNAME_COLOR = COLOR_INK_LIGHT_GREEN;

#define CELL_HALF_HEIGHT
#define CENTER_SCORE
