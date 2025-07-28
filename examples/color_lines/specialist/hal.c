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
#include "graph.h"
#include "graph_functions.h"

static uint8_t *const TEMP_SCREEN = (uint8_t *)0x4800;
static const uint8_t PLAYFIELD_Y = 29;
static const uint8_t PLAYFIELD_X = 11;
static const uint8_t HISCORE_X = 24;
static const uint8_t HISCORE_Y = 4;

static void ShowScreenSlowly2(const void *packedScreen, const void *packedScreenColors) {
    SET_COLOR(COLOR_INK_BLACK);
    UnpackMegalz(SCREEN, packedScreen);
    UnpackMegalz(TEMP_SCREEN, packedScreenColors);
    ShowScreenSlowly((unsigned)TEMP_SCREEN >> 8);
}

void DrawScoreAndCreatures(const char *scoreText) {
    DrawText(TILE(40, 7), 2, COLOR_INK_GREEN, scoreText);

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
            DrawImageTile(s, imgPlayerD, 5 * 256 + 4);
        DrawImageTile(s - 46, imgPlayer, 5 * 256 + 50);
        if (playerLevel == 14) {
            DrawImageTile(s - 50 + 0x200, imgPlayerWin, 3 * 256 + 16);
            DrawImageTile(TILE(3, 53), imgKingLose, 6 * 256 + 62);
        }  // TODO: Торчит остаток меча
    }
}

static void HideScreenSlowly(void) {
    memset(TEMP_SCREEN, COLOR_INK_BLACK, SCREEN_SIZE);
    ShowScreenSlowly((unsigned)TEMP_SCREEN >> 8);
}

void Intro(void) {
    ShowScreenSlowly2(imgTitle, imgTitle_colors);
}

void DrawScreen(const char *scoreText) {
    HideScreenSlowly();
    ShowScreenSlowly2(imgScreen, imgScreen_colors);
    DrawTextXY(56, 19, COLOR_INK_GREEN, "Вы");
    DrawText(TILE(3, 7), 1, COLOR_INK_GREEN, scoreText);
    uint16_t x = (9 - strlen(hiScores[0].name)) / 2 + 3;
    DrawTextXY(x, 19, COLOR_INK_GREEN, hiScores[0].name);
}

static void DrawBall(uint8_t *tile, const uint8_t *image, uint8_t c) {
    static const uint8_t colors[1] = {COLOR_INK_BLUE, COLOR_INK_WHITE, COLOR_INK_YELLOW, COLOR_INK_VIOLET,
                                      COLOR_INK_RED,  COLOR_INK_CYAN,  COLOR_INK_GREEN};
    DrawImageTileMono(tile, image, 0x20E, colors[c - 1]);
}

static uint8_t *CellAddress(uint8_t x, uint8_t y) {
    return TILE(PLAYFIELD_X + x * 3, PLAYFIELD_Y + y * 20);
}

static void DrawBallFast(uint8_t x, uint8_t y, const uint8_t *o, uint8_t c) {
    register uint8_t *d = CellAddress(x, y);
    SET_COLOR(COLOR_INK_BLUE);
    d[-1] = 0x55;
    d[0x100 - 1] = 0x55;
    DrawBall(d, o, c);
}

void DrawCell(uint8_t x, uint8_t y, uint8_t color) {
    if (color == 0)
        DrawBallFast(x, y, imgBalls + 28 * 16, 1);
    else
        DrawBallFast(x, y, imgBalls, color);
}

void DrawSpriteRemove(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    static const uint8_t *const images[4] = {
        imgBalls + 5 * 28,
        imgBalls + 6 * 28,
        imgBalls + 7 * 28,
        imgBalls + 8 * 28,
    };
    DrawBallFast(x, y, images[phase], color);
}

void DrawSpriteNew(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    DrawBallFast(x, y, imgBalls + (4 - phase) * 28, color);
}

void DrawSpriteStep(uint8_t x, uint8_t y, uint8_t color) {
    DrawBallFast(x, y, imgBalls + (12 + color) * 28, 1);
}

void DrawBouncingBall(uint8_t x, uint8_t y, uint8_t color, uint8_t phase, bool cursor) {
    static const uint8_t *const selAnimation[6] = {imgBalls,           imgBalls,           imgBalls,
                                                   imgBalls + 28 * 10, imgBalls + 28 * 11, imgBalls + 28 * 10};

    uint8_t *d;
    if (phase == 1) {
        d = CellAddress(x, y) - 1;
        SET_COLOR(COLOR_INK_BLUE);
        d[14] = 0x55;
        d[0x100 + 14] = 0x55;
        DrawBall(d, selAnimation[phase], color);
    } else {
        DrawBallFast(x, y, selAnimation[phase], color);
    }

    if (cursor)
        DrawCursor();
}

void DrawHelp(const uint8_t *newBalls) {
    DrawBall(TILE(20, 3), imgBalls, newBalls[0]);
    DrawBall(TILE(23, 3), imgBalls, newBalls[1]);
    DrawBall(TILE(26, 3), imgBalls, newBalls[2]);
}

void DrawCursor(void) {
    SET_COLOR(COLOR_INK_YELLOW);
    uint8_t *a = CellAddress(cursorX, cursorY);
    memset(a, 255, 2);
    memset(a + 0x100, 255, 2);
    memset(a + 12, 255, 2);
    memset(a + 0x100 + 12, 255, 2);
}

void ClearCursor(void) {
    DrawCell(cursorX, cursorY, game[cursorX][cursorY]);
}

static void DrawButton(uint8_t *tile, bool state) {
    SET_COLOR(state ? COLOR_INK_GREEN : COLOR_INK_BLUE);

    uint8_t i;
    for (i = 4; i; --i, tile += 0x100)
        memcpy(tile, tile, 8);  // TODO: ChangeColor
}

void DrawButtons(void) {
    DrawButton(TILE(10, 240), showPath);
    DrawButton(TILE(18, 240), soundEnabled);
    DrawButton(TILE(26, 240), showHelp);
}

void DrawHiScoresItem(uint8_t y, uint8_t colorNumber, const char *text) {
    static const uint8_t colors[] = {
        COLOR_INK_CYAN,
        COLOR_INK_RED,
        COLOR_INK_YELLOW,
        COLOR_INK_WHITE,
    };
    DrawTextXY(HISCORE_X, (HISCORE_Y + 1) + y, colors[colorNumber], text);
}

void DrawHiScoresWindow(void) {
    SET_COLOR(COLOR_INK_BLUE);
    graph0();
    FillRectFast(FILLRECTARGS(108, PLAYFIELD_Y + 17, 274, PLAYFIELD_Y + 156));
}
