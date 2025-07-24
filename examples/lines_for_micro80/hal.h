/*
 * Game "Color Lines" for Micro 80
 * Copyright (c) 2025 Aleksey Morozov
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
#include <terminal.h>

#define TEXTCOORDS(X, Y) (uint8_t *)(0xE800 + (X) + (Y)*64)

void PlaySoundJump(void);
void PlaySoundCantMove(void);
void DrawEmptyCell(uint8_t x, uint8_t y);
void DrawCell1(uint8_t *graphAddr, uint8_t color);
void DrawCell(uint8_t x, uint8_t y, uint8_t color);
static const uint16_t REMOVE_ANIMATION_DELAY = 2000; /* задерка в main */
static const uint8_t REMOVE_ANIMATION_COUNT = 3;     /* phase для DrawSpriteRemove */
void DrawSpriteRemove(uint8_t x, uint8_t y, uint8_t c, uint8_t n);
static const uint16_t NEW_ANIMATION_DELAY = 2000; /* задерка в main */
static const uint8_t NEW_ANIMATION_COUNT = 5;     /* phase для DrawSpriteNew */
void DrawSpriteNew1(uint8_t *graphAddress, uint8_t color, uint8_t phase);
void DrawSpriteNew(uint8_t x, uint8_t y, uint8_t color, uint8_t phase);
static const uint16_t STEP_ANIMATION_DELAY = 4000; /* задерка в main */
void DrawSpriteStep(uint8_t x, uint8_t y, uint8_t color);
static const uint16_t BOUNCE_ANIMATION_DELAY = 50; /* задерка в main */
static const uint8_t BOUNCE_ANIMATION_COUNT = 6;   /* phase для DrawBouncingBall */
void DrawBouncingBall(uint8_t x, uint8_t y, uint8_t color, uint8_t phase);
void DrawHelp(void);
void DrawCursor(void);
void ClearCursor(void);
static const unsigned BUTTON_WIDTH = 13;
static uint8_t *const BUTTON_PATH = TEXTCOORDS(2, 22);  /* для state в DrawButton */
static uint8_t *const BUTTON_SOUND = TEXTCOORDS(2, 23); /* для state в DrawButton */
static uint8_t *const BUTTON_HELP = TEXTCOORDS(2, 24);  /* для state в DrawButton */
void DrawButton(uint8_t *graphAddress, bool state);
void ChangePalette(void);
void Intro(void);
static const uint16_t HISCORE_ANIMATION_DELAY = 2000; /* задерка в main */
void DrawHiScoresScreen1(uint8_t i, uint8_t pos);
void DrawHiScores(bool enterNameMode);
void DrawHiScoresScreen(uint8_t pos);
void DrawHiScoresLastLine(void);
void DrawScoreAndCreatures(void);
void DrawScreen(void);
uint8_t ReadKeyboard(bool noWait);
