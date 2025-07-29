/*
 * Game "Color Lines" for Micro 80
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
#include <stdbool.h>
#include <hal/hal.h>

static const uint8_t REMOVE_ANIMATION_COUNT = 3; /* phase для DrawSpriteRemove */
static const uint8_t NEW_ANIMATION_COUNT = 5;    /* phase для DrawSpriteNew */
static const uint8_t BOUNCE_ANIMATION_COUNT = 6; /* phase для DrawBouncingBall */

void DrawCell(uint8_t x, uint8_t y, uint8_t color);
void DrawSpriteRemove(uint8_t x, uint8_t y, uint8_t color, uint8_t phase);
void DrawSpriteNew(uint8_t x, uint8_t y, uint8_t color, uint8_t phase);
void DrawSpriteStep(uint8_t x, uint8_t y, uint8_t color);
void DrawBouncingBall(uint8_t x, uint8_t y, uint8_t color, uint8_t phase);
void DrawHelp(const uint8_t *newBalls);
void DrawCursor(void);
void ClearCursor(void);
void DrawButtons(void);
void Intro(void);
void DrawHiScoresWindow(void);
void DrawHiScoresItem(uint8_t y, uint8_t colorNumber, const char *text);
void DrawScore(const char *scoreText);
void DrawScreen(const char *scoreText);
uint8_t ReadKeyboard(bool noWait);
