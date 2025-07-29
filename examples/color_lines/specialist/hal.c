/*
 * Game "Color Lines" for 8-bit computer Specialist
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
#include <codepage/866.h>
#include <c8080/unpackmegalz.h>
#include <c8080/delay.h>
#include <c8080/uint16tostring.h>
#include <hal/hal.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "resources.h"
#include "fillrect.h"
#include "drawtext.h"

static uint8_t *const TEMP_SCREEN = (uint8_t *)0x4800;
static const uint8_t PLAYFIELD_Y = 29;
static const uint8_t PLAYFIELD_X = 11;
static const uint8_t HISCORE_X = 72;
static const uint8_t HISCORE_Y = 57;

static void ShowScreenSlowly2(const void *packedScreen, const void *packedScreenColors) {
    SET_COLOR(COLOR_INK_BLACK);
    UnpackMegalz(SCREEN, packedScreen);
    UnpackMegalz(TEMP_SCREEN, packedScreenColors);
    ShowScreenSlowly((unsigned)TEMP_SCREEN >> 8);
}

void DrawScore(const char *scoreText) {
    SET_COLOR(COLOR_INK_GREEN);
    DrawText(DRAWTEXTARGS(162, 8), scoreText);

    uint8_t n;
    if (score < hiScores[0].score) {
        n = (score / (hiScores[0].score / 13));
        if (n > 13)
            n = 13;
    } else {
        n = 14;
    }

    if (playerLevel != n) {
        playerLevel = n;
        uint8_t *s;
        for (s = TILE(40, 168); n; --n, s -= 4)
            DrawImageTile(s, imgPlayerD, imgPlayerDSize);
        DrawImageTile(s - 46, imgPlayer, imgPlayerSize);
        if (playerLevel == 14) {
            DrawImageTile(s - 50 + 0x200, imgPlayerWin, imgPlayerWinSize);
            DrawImageTile(TILE(3, 53), imgKingLose, imgKingLoseSize);
        }
    }
}

static void HideScreenSlowly(void) {
    memset(TEMP_SCREEN, COLOR_INK_BLACK, SCREEN_SIZE);
    ShowScreenSlowly((unsigned)TEMP_SCREEN >> 8);
}

void Intro(void) {
    ShowScreenSlowly2(imgTitle, imgTitleColors);
}

void DrawScreen(const char *scoreText) {
    HideScreenSlowly();
    ShowScreenSlowly2(imgScreen, imgScreenColors);
    SET_COLOR(COLOR_INK_GREEN);
    DrawText(DRAWTEXTARGS(168, 190), "Вы");
    DrawText(DRAWTEXTARGS(13, 8), scoreText);
    uint16_t x = (9 - strlen(hiScores[0].name)) * 3 / 2 + 9;
    DrawTextXY(x, 190, hiScores[0].name);
}

static void DrawBall(uint8_t *tile, const uint8_t *image, uint8_t c) {
    static const uint8_t colors[] = {COLOR_INK_BLACK,  COLOR_INK_BLUE, COLOR_INK_WHITE, COLOR_INK_YELLOW,
                                     COLOR_INK_VIOLET, COLOR_INK_RED,  COLOR_INK_CYAN,  COLOR_INK_GREEN};
    SET_COLOR(colors[c]);
    DrawImageTileMono(tile, image, imgBallsSize);
}

static uint8_t *CellAddress(uint8_t x, uint8_t y) {
    return TILE(PLAYFIELD_X + x * 3, PLAYFIELD_Y + y * 20);
}

static void DrawBallXY(uint8_t x, uint8_t y, const uint8_t *image, uint8_t color) {
    uint8_t *d = CellAddress(x, y);
    SET_COLOR(COLOR_INK_BLUE);
    d[-1] = 0x55;
    d[0x100 - 1] = 0x55;
    DrawBall(d, image, color);
}

static void DrawBallJumpXY(uint8_t x, uint8_t y, const uint8_t *image, uint8_t color) {
    uint8_t *d = CellAddress(x, y) - 1;
    SET_COLOR(COLOR_INK_BLUE);
    d[imgBallsHeight] = 0x55;
    d[0x100 + imgBallsHeight] = 0x55;
    DrawBall(d, image, color);
}

void DrawCell(uint8_t x, uint8_t y, uint8_t color) {
    if (color == 0)
        DrawBallXY(x, y, imgBalls[16], 1);
    else
        DrawBallXY(x, y, imgBalls[0], color);
}

void DrawSpriteRemove(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    static const uint8_t *const images[4] = {imgBalls[5], imgBalls[6], imgBalls[7], imgBalls[8]};
    DrawBallXY(x, y, images[phase], color);
}

void DrawSpriteNew(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    DrawBallXY(x, y, imgBalls[4 - phase], color);
}

void DrawSpriteStep(uint8_t x, uint8_t y, uint8_t color) {
    DrawBallXY(x, y, imgBalls[12 + color], 1);
}

void DrawBouncingBall(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    static const uint8_t *const selAnimation[6] = {imgBalls[0],  imgBalls[0],  imgBalls[0],
                                                   imgBalls[10], imgBalls[11], imgBalls[10]};

    if (phase == 1)
        DrawBallJumpXY(x, y, imgBalls[0], color);
    else
        DrawBallXY(x, y, selAnimation[phase], color);

    if (selX == cursorX && selY == cursorY)
        DrawCursor();
}

void DrawHelp(const uint8_t *newBalls) {
    DrawBall(TILE(20, 4), imgBalls[0], newBalls[0]);
    DrawBall(TILE(23, 4), imgBalls[0], newBalls[1]);
    DrawBall(TILE(26, 4), imgBalls[0], newBalls[2]);
}

void DrawCursor(void) {
    SET_COLOR(COLOR_INK_YELLOW);
    uint8_t *a = CellAddress(cursorX, cursorY);
    memset(a, 255, 2);  // TODO: Cursor image
    memset(a + 0x100, 255, 2);
    memset(a + 12, 255, 2);
    memset(a + 0x100 + 12, 255, 2);
}

void ClearCursor(void) {
    DrawCell(cursorX, cursorY, game[cursorX][cursorY]);
}

static void DrawButton(uint8_t *tile, bool state) {
    SET_COLOR(state ? COLOR_INK_GREEN : COLOR_INK_BLUE);
    ChangeTileColor(tile, 4, 8);
}

void DrawButtons(void) {
    DrawButton(TILE(10, 240), showPath);
    DrawButton(TILE(18, 240), soundEnabled);
    DrawButton(TILE(26, 240), showHelp);
}

void DrawHiScoresWindow(void) {
    SET_COLOR(COLOR_INK_BLUE);
    FillRectInverse();
    FillRect(FILLRECTARGS(108, PLAYFIELD_Y + 17, 274, PLAYFIELD_Y + 156));
}

void DrawHiScoresItem(uint8_t y, uint8_t colorNumber, const char *text) {
    static const uint8_t colors[] = {
        COLOR_INK_CYAN,
        COLOR_INK_RED,
        COLOR_INK_YELLOW,
        COLOR_INK_WHITE,
    };
    SET_COLOR(colors[colorNumber]);
    DrawTextXY(HISCORE_X, HISCORE_Y + y * 10, text);
}
