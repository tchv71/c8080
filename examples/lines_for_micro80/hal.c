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

#include "hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <c8080/delay.h>
#include <c8080/div16mod.h>
#include <c8080/uint16tostring.h>
#include "graph/imgBalls.h"
#include "graph/imgBoard.h"
#include "graph/imgScreen.h"
#include "graph/imgPlayer.h"
#include "graph/imgPlayerWin.h"
#include "graph/imgKingLose.h"
#include "graph/imgTitle.h"
#include "lines.h"

static const uint8_t CELL_WIDTH = 3;
static const uint8_t CELL_HEIGHT = 3;
static const uint8_t PLAYFIELD_X = (TEXT_WIDTH - CELL_WIDTH * GAME_WIDTH) / 2;
static const uint8_t PLAYFIELD_Y = 0;
static const uint8_t HISCORE_X = 22;
static const uint8_t HISCORE_Y = 5;
static const uint8_t RIGHT_CREATURE_Y_MAX = 3;

static uint8_t rightCreatureY;

void Intro(void) {
    ClearScreenBlack();
    memcpy((void *)0xE800, imgTitleC, sizeof(imgTitleC)); // TODO: Replace drawImage
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

void DrawScreen(void) {
    ClearScreenBlack();
    memcpy((void *)0xE800, imgScreenC, sizeof(imgScreenC)); // TODO: Replace drawImage
    memcpy((void *)0xE000, imgScreenA, sizeof(imgScreenA));

    DrawButtons();
    DrawText(TILE(2, 25), COLOR_INK_LIGHT_CYAN | COLOR_PAPER_WHITE, "4 Зал славы");
    DrawText(TILE(2, 26), COLOR_INK_LIGHT_CYAN | COLOR_PAPER_WHITE, "5 Новая игра");

    rightCreatureY = -1; // Redraw
    DrawScoreAndCreatures();

    char scoreText[UINT16_TO_STRING_SIZE + 1];
    Uint16ToString(scoreText, hiScores[0].score, 10);
    DrawText(TILE(7, 1), COLOR_INK_LIGHT_GREEN, scoreText);

    DrawHelp();

    uint8_t x, y;
    uint8_t *a = &game[0][0];
    for (x = 0; x < GAME_WIDTH; x++)
        for (y = 0; y < GAME_HEIGHT; y++)
            DrawCell(x, y, *a++);

    DrawCursor();
}

static void DrawTextXY(uint8_t x, uint8_t y, uint8_t color, const char *text) {
    DrawText(TILE(x, y), color, text);
}

void PlaySoundJump(void) {
}

void PlaySoundCantMove(void) {
}

static uint8_t *CellAddress(uint8_t x, uint8_t y) {
    return TILE(PLAYFIELD_X + x * 3, PLAYFIELD_Y + y * 3);
}

static void DrawBall1(uint8_t *graphAddr, uint8_t *image, uint8_t color) {
    DrawImageTile(graphAddr, image + (sizeof(imgBoard[0]) * 10) * (color - 1), imgBoardSize);
}

static void DrawEmptyCell1(uint8_t *graphAddr) {
    DrawImageTile(graphAddr, imgBoard[4], imgBoardSize);
}

void DrawEmptyCell(uint8_t x, uint8_t y) {
    DrawEmptyCell1(CellAddress(x, y));
}

static void DrawCell1(uint8_t *addr, uint8_t color) {
    DrawImageTile(addr, color ? imgBalls[10 * (color - 1)] : imgBoard[4], imgBoardSize);
}

void DrawCell(uint8_t x, uint8_t y, uint8_t color) {
    DrawCell1(CellAddress(x, y), color);
}

static const uint8_t *removeAnimationImages[3] = {
    imgBalls[5],
    imgBalls[6],
    imgBalls[7],
};

void DrawSpriteRemove(uint8_t x, uint8_t y, uint8_t c, uint8_t phase) {
    DrawImageTile(CellAddress(x, y), removeAnimationImages[phase] + (sizeof(imgBoard[0]) * 10) * (c - 1), imgBoardSize);
}

static void DrawSpriteNew1(uint8_t *graphAddress, uint8_t color, uint8_t phase) {
    DrawImageTile(graphAddress, imgBalls[10 * (color - 1) + (4 - phase)],
              imgBoardSize);
}

void DrawSpriteNew(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    DrawSpriteNew1(CellAddress(x, y), color, phase);
}

void DrawSpriteStep(uint8_t x, uint8_t y, uint8_t color) {
    DrawImageTile(CellAddress(x, y), imgBoard[color], imgBoardSize);
    //    if (soundEnabled) {
    //        PLAY_SOUND_TICK
    //    }
}

static uint8_t *bouncingAnimation[6] = {imgBalls[0], imgBalls[9], imgBalls[9],
                                        imgBalls[0], imgBalls[8], imgBalls[8]};

void DrawBouncingBall(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    uint8_t *graphAddress = CellAddress(x, y);
    DrawBall1(graphAddress, bouncingAnimation[phase], color);
}

static uint8_t *const helpCoords[NEW_BALL_COUNT] = {
    TILE(49, 23),
    TILE(53, 23),
    TILE(57, 23),
};

void DrawHelp(void) {
    uint8_t i;
    for (i = 0; i < NEW_BALL_COUNT; i++) {
        if (showHelp)
            DrawSpriteNew1(helpCoords[i], newBalls[i], 4);
        else
            DrawImageTile(helpCoords[i], imgBoard[5], imgBoardSize);
    }
}

void DrawCursor(void) {
    uint8_t *a = CellAddress(cursorX, cursorY);
    a[65] = '*';
    a[65 - 0x800] = COLOR_INK_LIGHT_YELLOW;
}

void ClearCursor(void) {
    DrawCell(cursorX, cursorY, game[cursorX][cursorY]);
}

static void DrawHiScoresScreen1(uint8_t i, uint8_t pos) {
    struct HiScore *h = hiScores + i;
    for (; i < HISCORE_COUNT; ++i) {
        char text[10 + 1 + 5 + 1];
        h->name[9] = '\0';
        snprintf(text, sizeof(text), "%10s %5u", h->name, h->score);
        DrawTextXY(HISCORE_X + 2, (HISCORE_Y + 3) + i, (pos == i) ? (COLOR_INK_LIGHT_RED | COLOR_PAPER_BLUE) : (COLOR_INK_LIGHT_CYAN | COLOR_PAPER_BLUE), text);
        h++;
    }
}

static void DrawHiScoresLast(const char *text) {
    DrawText(TILE(HISCORE_X + 1, HISCORE_Y + HISCORE_COUNT + 4),
             COLOR_INK_LIGHT_YELLOW | COLOR_PAPER_BLUE,
             text);
}

void DrawHiScoresSpace(void) {
    DrawHiScoresLast("  Нажмите Пробел  ");
}

void DrawWindow(uint8_t x, uint8_t y, uint8_t h, uint8_t color) {
    DrawTextXY(x, y, color, "┌──────────────────┐");
    y++;
    do {
        DrawTextXY(x, y, color, "│                  │");
        y++;
        h--;
    } while (h > 2);
    DrawTextXY(x, y, color, "└──────────────────┘");
}

void DrawHiScores(bool enterNameMode) {
    DrawWindow((TEXT_WIDTH - 20) / 2, HISCORE_Y, HISCORE_COUNT + 6, COLOR_INK_LIGHT_WHITE | COLOR_PAPER_BLUE);
    DrawTextXY((TEXT_WIDTH - 8) / 2, HISCORE_Y + 1, COLOR_INK_LIGHT_YELLOW | COLOR_PAPER_BLUE, "Зал славы");
    if (enterNameMode)
        DrawHiScoresLast(" Введите свое имя "); // TODO: ё
    else
        DrawHiScoresSpace();
    DrawHiScoresScreen1(0, enterNameMode ? HISCORE_COUNT - 1 : HISCORE_COUNT);
}

void DrawHiScoresScreen(uint8_t pos) {
    DrawHiScoresScreen1(0, pos);
}

void DrawHiScoresLastLine(void) {
    DrawHiScoresScreen1(HISCORE_COUNT - 1, HISCORE_COUNT - 1);
}

void DrawScoreAndCreatures(void) {
    char scoreText[UINT16_TO_STRING_SIZE + 1];
    Uint16ToString(scoreText, score, 10);
    DrawText(TILE(52, 1), COLOR_INK_LIGHT_GREEN, scoreText);

    uint8_t n;
    if (score < hiScores[0].score) {
        n = score / (hiScores[0].score / (RIGHT_CREATURE_Y_MAX - 1));
        if (n > RIGHT_CREATURE_Y_MAX - 1)
            n = RIGHT_CREATURE_Y_MAX - 1;
    } else {
        n = RIGHT_CREATURE_Y_MAX;
    }

    while (rightCreatureY != n) {
        rightCreatureY++;
        DrawImageTile(TILE(48, 7 - rightCreatureY), imgPlayer, imgPlayerSize);
        if (rightCreatureY == RIGHT_CREATURE_Y_MAX) {
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
