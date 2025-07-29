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

#include "../hal.h"
#include "../lines.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <c8080/delay.h>
#include <c8080/div16mod.h>
#include <c8080/uint16tostring.h>
#include "imgBalls.h"
#include "imgBoard.h"
#include "imgScreen.h"
#include "imgPlayer.h"
#include "imgPlayerWin.h"
#include "imgKingLose.h"
#include "imgTitle.h"

static const uint8_t CELL_WIDTH = 3;
static const uint8_t CELL_HEIGHT = 3;
static const uint8_t PLAYFIELD_X = (TEXT_WIDTH - CELL_WIDTH * GAME_WIDTH) / 2;
static const uint8_t PLAYFIELD_Y = 0;
static const uint8_t HISCORE_X = 22;
static const uint8_t HISCORE_Y = 5;
static const uint8_t RIGHT_CREATURE_Y_MAX = 3;

void Intro(void) {
    ResetScreen();
    memcpy((void *)0xE800, imgTitleC, sizeof(imgTitleC));  // TODO: Replace drawImage
    memcpy((void *)0xE000, imgTitleA, sizeof(imgTitleA));
    sleep(1);
    getchar();
}

static uint8_t GetButtonColor(bool enabled) {
    return enabled ? (COLOR_INK_LIGHT_CYAN | COLOR_PAPER_WHITE) : (COLOR_INK_GRAY | COLOR_PAPER_WHITE);
}

void DrawButtons(void) {
    DrawText(TILE(2, 22), GetButtonColor(showPath), "1 Путь");
    DrawText(TILE(2, 23), GetButtonColor(soundEnabled), "2 Звук");
    DrawText(TILE(2, 24), GetButtonColor(showHelp), "3 Подсказка");
}

void DrawScreen(const char *scoreText) {
    ResetScreen();
    memcpy((void *)0xE800, imgScreenC, sizeof(imgScreenC));  // TODO: Replace drawImage
    memcpy((void *)0xE000, imgScreenA, sizeof(imgScreenA));

    DrawText(TILE(2, 25), COLOR_INK_LIGHT_CYAN | COLOR_PAPER_WHITE, "4 Зал славы");
    DrawText(TILE(2, 26), COLOR_INK_LIGHT_CYAN | COLOR_PAPER_WHITE, "5 Новая игра");
    DrawText(TILE(7, 1), COLOR_INK_LIGHT_GREEN, scoreText);
}

static void DrawTextXY(uint8_t x, uint8_t y, uint8_t color, const char *text) {
    DrawText(TILE(x, y), color, text);
}

static uint8_t *CellAddress(uint8_t x, uint8_t y) {
    return TILE(PLAYFIELD_X + x * 3, PLAYFIELD_Y + y * 3);
}

static void DrawBall1(uint8_t *tile, const uint8_t *image, uint8_t color) {
    DrawImageTile(tile, image + (sizeof(imgBoard[0]) * 10) * (color - 1), imgBoardSize);
}

static void DrawCell1(uint8_t *tile, uint8_t color) {
    DrawImageTile(tile, color ? imgBalls[10 * (color - 1)] : imgBoard[4], imgBoardSize);
}

void DrawCell(uint8_t x, uint8_t y, uint8_t color) {
    DrawCell1(CellAddress(x, y), color);
}

void DrawSpriteRemove(uint8_t x, uint8_t y, uint8_t c, uint8_t phase) {
    static const uint8_t *const images[3] = {
        imgBalls[5],
        imgBalls[6],
        imgBalls[7],
    };
    DrawImageTile(CellAddress(x, y), images[phase] + (sizeof(imgBoard[0]) * 10) * (c - 1), imgBoardSize);
}

static void DrawSpriteNew1(uint8_t *graphAddress, uint8_t color, uint8_t phase) {
    DrawImageTile(graphAddress, imgBalls[10 * (color - 1) + (4 - phase)], imgBoardSize);
}

void DrawSpriteNew(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    DrawSpriteNew1(CellAddress(x, y), color, phase);
}

void DrawSpriteStep(uint8_t x, uint8_t y, uint8_t color) {
    DrawImageTile(CellAddress(x, y), imgBoard[color], imgBoardSize);
}

void DrawBouncingBall(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    static const uint8_t *const images[] = {
        imgBalls[0], imgBalls[9], imgBalls[9], imgBalls[0], imgBalls[8], imgBalls[8],
    };
    DrawBall1(CellAddress(x, y), images[phase], color);
    if (selX == cursorX && selY == cursorY)
        DrawCursor();
}

void DrawHelp(const uint8_t *newBalls) {
    DrawSpriteNew1(TILE(49, 23), newBalls[0], 4);
    DrawSpriteNew1(TILE(53, 23), newBalls[1], 4);
    DrawSpriteNew1(TILE(57, 23), newBalls[2], 4);
}

void DrawCursor(void) {
    uint8_t *a = CellAddress(cursorX, cursorY);
    a[65] = '*';  // TODO
    a[65 - 0x800] = COLOR_INK_LIGHT_YELLOW;
}

void ClearCursor(void) {
    DrawCell(cursorX, cursorY, game[cursorX][cursorY]);
}

static void DrawWindow(uint8_t x, uint8_t y, uint8_t h, uint8_t color) {
    DrawTextXY(x, y, color, "┌──────────────────┐");
    y++;
    do {
        DrawTextXY(x, y, color, "│                  │");
        y++;
        h--;
    } while (h > 2);
    DrawTextXY(x, y, color, "└──────────────────┘");
}

void DrawHiScoresWindow(void) {
    DrawWindow((TEXT_WIDTH - 20) / 2, HISCORE_Y, HISCORE_COUNT + 6, COLOR_INK_LIGHT_WHITE | COLOR_PAPER_BLUE);
}

void DrawHiScoresItem(uint8_t y, uint8_t colorNumber, const char *text) {
    static const uint8_t colors[] = {
        COLOR_INK_LIGHT_CYAN | COLOR_PAPER_BLUE,
        COLOR_INK_LIGHT_RED | COLOR_PAPER_BLUE,
        COLOR_INK_LIGHT_YELLOW | COLOR_PAPER_BLUE,
        COLOR_INK_LIGHT_WHITE | COLOR_PAPER_BLUE,
    };
    DrawTextXY(HISCORE_X + 2, (HISCORE_Y + 1) + y, colors[colorNumber], text);
}

void DrawScore(const char *scoreText) {
    DrawText(TILE(52, 1), COLOR_INK_LIGHT_GREEN, scoreText);

    uint8_t n;
    if (score < hiScores[0].score) {
        n = score / (hiScores[0].score / (RIGHT_CREATURE_Y_MAX - 1));
        if (n > RIGHT_CREATURE_Y_MAX - 1)
            n = RIGHT_CREATURE_Y_MAX - 1;
    } else {
        n = RIGHT_CREATURE_Y_MAX;
    }

    while (playerLevel != n) {
        playerLevel++;
        DrawImageTile(TILE(48, 7 - playerLevel), imgPlayer, imgPlayerSize);
        if (playerLevel == RIGHT_CREATURE_Y_MAX) {
            DrawImageTile(TILE(53, 4), imgPlayerWin, imgPlayerWinSize);
            DrawImageTile(TILE(4, 4), imgKingLose, imgKingLoseSize);
        }
    }
}

uint8_t ReadKeyboard(bool noWait) {
    if (noWait)
        return ScanKey();
    return getchar();
}
