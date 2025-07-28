#include "../hal.h"
#include "../lines.h"
#include <codepage/koi7.h>
#include <c8080/unpackmegalz.h>
#include <c8080/delay.h>
#include <c8080/uint16tostring.h>
#include <hal/hal.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "graph.h"
#include "graph_functions.h"

static uint8_t* const TEMP_SCREEN = (uint8_t*)0x4800;
static const uint8_t PLAYFIELD_Y = 29;
static const uint8_t PLAYFIELD_X = 11;

uint8_t playerSpriteTop;

static void ShowScreenSlowly2(const void *packed_screen, const void *packed_screen_colors) {
    SET_COLOR(COLOR_INK_BLACK);
    UnpackMegalz(SCREEN, packed_screen);
    UnpackMegalz(TEMP_SCREEN, packed_screen_colors);
    ShowScreenSlowly((unsigned)TEMP_SCREEN >> 8);
}

void DrawScoreAndCreatures(void) {
    char scoreText[UINT16_TO_STRING_SIZE + 1];
    Uint16ToString(scoreText, score, 10);
    SET_COLOR(COLOR_INK_GREEN);
    DrawText6(TILE(40, 7), 2, 5, scoreText);

    uint8_t n;
    if(score < hiScores[0].score) {
        n = (score / (hiScores[0].score / 13));
        if(n>13)
            n=13;
    } else {
        n = 14;
    }

    if(playerSpriteTop != n) {
        playerSpriteTop = n;
        uint8_t* s;
        for(s=TILE(40, 168); n; --n, s-=4)
            DrawImageTile(s, imgPlayerD, 5*256+4);
        DrawImageTile(s-46, imgPlayer, 5*256+50);
        if(playerSpriteTop == 14) {
            DrawImageTile(s-50+0x200, imgPlayerWin, 3*256+16);
            DrawImageTile(TILE(3, 53), imgKingLose, 6*256+62);
        } // TODO: Торчит остаток меча
    }
}

static void HideScreenSlowly(void) {
    memset(TEMP_SCREEN, COLOR_INK_BLACK, SCREEN_SIZE);
    ShowScreenSlowly((unsigned)TEMP_SCREEN >> 8);
}

void Intro(void) {
    ShowScreenSlowly2(imgTitle, imgTitle_colors);
}

static void UpdateTopScore(void) {
    char scoreText[UINT16_TO_STRING_SIZE + 1];
    Uint16ToString(scoreText, hiScores[0].score, 10);
    SET_COLOR(COLOR_INK_GREEN);
    DrawText6(TILE(3, 7), 1, 5, scoreText);

    DrawText6(PRINTARGS(5, 19), 7|0x80, hiScores[0].name);
}

void DrawScreen(void) {
    HideScreenSlowly();
    ShowScreenSlowly2(imgScreen, imgScreen_colors);

    SET_COLOR(COLOR_INK_GREEN);
    DrawTextXY(56,19,"Вы");

    UpdateTopScore();

    playerSpriteTop = -1;
    DrawScoreAndCreatures();
}

static const uint8_t colors[1] = {
    COLOR_INK_BLUE,
    COLOR_INK_WHITE,
    COLOR_INK_YELLOW,
    COLOR_INK_VIOLET,
    COLOR_INK_RED,
    COLOR_INK_CYAN,
    COLOR_INK_GREEN,
    COLOR_INK_BLACK
};

static void drawBall1(uint8_t* d, const uint8_t* o, uint8_t c) {
  SET_COLOR(colors[c-1]);
  DrawImageTileMono(d, o, 0x20E);
}

static uint8_t* cellAddr(uint8_t x, uint8_t y) {
  return TILE(PLAYFIELD_X + x * 3, PLAYFIELD_Y + y * 20);
}

static void DrawBallFast(uint8_t x, uint8_t y, const uint8_t* o, uint8_t c) {
  register uint8_t* d = cellAddr(x, y);
  SET_COLOR(COLOR_INK_BLUE);
  d[-1] = 0x55;
  d[0x100-1] = 0x55;
  drawBall1(d, o, c);
}

void DrawCell(uint8_t x, uint8_t y, uint8_t color) {
    if (color == 0) DrawBallFast(x, y, imgBalls+28*16, 1);
               else DrawBallFast(x, y, imgBalls, color);
}

void DrawSpriteRemove(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    static const uint8_t* const images[4] = { imgBalls+5*28, imgBalls+6*28, imgBalls+7*28, imgBalls+8*28 };
    DrawBallFast(x, y, images[phase], color);
}

void DrawSpriteNew(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    DrawBallFast(x, y, imgBalls + (4 - phase) * 28, color);
}

void DrawSpriteStep(uint8_t x, uint8_t y, uint8_t color) {
    DrawBallFast(x, y, imgBalls + (12 + color) * 28, 1);
}

void DrawBouncingBall(uint8_t x, uint8_t y, uint8_t color, uint8_t phase, bool cursor) {
    static const uint8_t* const selAnimation[6] = { imgBalls, imgBalls, imgBalls, imgBalls+28*10, imgBalls+28*11, imgBalls+28*10 };

    uint8_t* d;
    if(phase==1) {
      d = cellAddr(x, y) - 1;
      SET_COLOR(COLOR_INK_BLUE);
      d[14] = 0x55;
      d[0x100+14] = 0x55;
      drawBall1(d, selAnimation[phase], color);
    } else {
      DrawBallFast(x, y, selAnimation[phase], color);
    }

    if(cursor) DrawCursor();

    if(phase==3) Sound(3, 10);
}

void DrawHelp(const uint8_t *newBalls) {
    drawBall1(TILE(20, 3), imgBalls, newBalls[0]);
    drawBall1(TILE(23, 3), imgBalls, newBalls[1]);
    drawBall1(TILE(26, 3), imgBalls, newBalls[2]);
}

void DrawCursor(void) {
    SET_COLOR(COLOR_INK_YELLOW);
    memset(cellAddr(cursorX, cursorY), 255, 2);
    memset(cellAddr(cursorX, cursorY)+0x100, 255, 2);
    memset(cellAddr(cursorX, cursorY)+12, 255, 2);
    memset(cellAddr(cursorX, cursorY)+0x100+12, 255, 2);
}

void ClearCursor(void) {
    DrawCell(cursorX, cursorY, game[cursorX][cursorY]);
}

static void DrawButton(uint8_t *tile, bool state) {
    if (state) SET_COLOR(COLOR_INK_GREEN); else SET_COLOR(COLOR_INK_BLUE);

    uint8_t i;
    for(i = 4; i; --i, tile+=0x100)
        memcpy(tile, tile, 8);
}

void DrawButtons(void) {
    DrawButton(TILE(10, 240), showPath);
    DrawButton(TILE(18, 240), soundEnabled);
    DrawButton(TILE(26, 240), showHelp);
}

static const uint8_t HISCORE_X = 24;
static const uint8_t HISCORE_Y = 4;

static void DrawHiScoresScreen1(uint8_t i, uint8_t pos) {
    struct HiScore *h = hiScores + i;
    for (; i < HISCORE_COUNT; ++i) {
        char text[10 + 1 + 5 + 1];
        h->name[9] = '\0';
        snprintf(text, sizeof(text), "%10s %5u", h->name, h->score);
        SET_COLOR(pos == i ? COLOR_INK_YELLOW : COLOR_INK_WHITE);
        DrawTextXY(HISCORE_X, (HISCORE_Y + 3) + i, text);
        h++;
    }
}

void DrawHiScoresLastLine(void) {
    DrawHiScoresScreen1(HISCORE_COUNT - 1, HISCORE_COUNT - 1);
}

static void DrawHiScoresLast(const char *text) {
    DrawTextXY(HISCORE_X - 1, HISCORE_Y + HISCORE_COUNT + 4, text);
}

void DrawHiScoresSpace(void) {
    DrawHiScoresLast("  Нажмите Пробел  ");
}

void DrawHiScores(bool enterNameMode) {
    SET_COLOR(COLOR_INK_BLUE);
    graph0();
    FillRectFast(FILLRECTARGS(108, PLAYFIELD_Y+17, 274, PLAYFIELD_Y+156));

    DrawTextXY((TEXT_WIDTH - 8) / 2, HISCORE_Y + 1, "Зал славы");
    if (enterNameMode)
        DrawHiScoresLast(" Введите свое имя "); // TODO: ё
    else
        DrawHiScoresSpace();

    DrawHiScoresScreen1(0, enterNameMode ? HISCORE_COUNT - 1 : HISCORE_COUNT);
}

void DrawHiScoresScreen(uint8_t pos) {
    DrawHiScoresScreen1(0, pos);
}
