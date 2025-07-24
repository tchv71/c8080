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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <c8080/delay.h>
#include <c8080/div16mod.h>
#include <c8080/uint16tostring.h>
#include <string.h>
#include <codepage/koi7.h>
#include <terminal.h>
#include "graph/imgBalls.h"
#include "graph/imgBoard.h"
#include "graph/imgScreen.h"
#include "graph/imgPlayer.h"
#include "graph/imgPlayerWin.h"
#include "graph/imgKingLose.h"
#include "lines.h"
#include "hal.h"

uint8_t text_color = 0x47;

void DrawText1(uint8_t *d, uint8_t n, const char *text) {
    uint8_t insertSpaces = n & 0x80;
    n &= 0x7F;
    while (n != 0) {
        uint8_t c = *text;
        if (c != 0) {
            ++text;
        } else if (!insertSpaces) {
            return;
        }
        *d = c;
        d[-0x800] = text_color;
        d++;
        --n;
    }
}

void DrawText(uint8_t x, uint8_t y, uint8_t n, const char *text) {
    DrawText1(TEXTCOORDS(x, y), n, text);
}

void DrawImage(uint8_t *destination, const uint8_t *source, uint16_t width_height) {
    asm {
__a_3_drawimage=__a_3_drawimage
        ld   bc, hl ; width, height
__a_1_drawimage=$+1
        ld   hl, 0 ; destination
__a_2_drawimage=$+1
        ld   de, 0 ; source
drawimage_l1:
        push bc
        push hl
drawimage_l2:
        ld   a, (de)
        inc  de
        ld   (hl), a

        ld   a, h
        sub  08h
        ld   h, a

        ld   a, (de)
        inc  de
        ld   (hl), a

        ld   a, h
        add  08h
        ld   h, a

        inc  hl

        dec  c
        jp   nz, drawimage_l2

        pop  hl

        ld   bc, 64
        add  hl, bc

        pop  bc

        dec  b
        jp   nz, drawimage_l1
    }
}

static const uint8_t PLAYFIELD_Y = 0;
static const uint8_t PLAYFIELD_X = (TEXT_WIDTH - 3 * 9) / 2;
static const uint8_t BALL_IMAGE_SIZEOF = 3 * 3 * 2;
static const uint16_t BALL_IMAGE_WH = 0x303;
static const uint8_t HISCORE_Y = 5;
static const uint8_t RIGHT_CREATURE_Y_MAX = 3;

static uint8_t rightCreatureY;

void PlaySoundJump(void) {
}

void PlaySoundCantMove(void) {
}

// Игровое поле

static uint8_t *CellAddress(uint8_t x, uint8_t y) {
    return TEXTCOORDS(PLAYFIELD_X + x * 3, PLAYFIELD_Y + y * 3);
}

static void DrawBall1(uint8_t *graphAddr, uint8_t *image, uint8_t color) {
    DrawImage(graphAddr, image + (BALL_IMAGE_SIZEOF * 10) * (color - 1), BALL_IMAGE_WH);
}

static void DrawEmptyCell1(uint8_t *graphAddr) {
    DrawImage(graphAddr, imgBoard + BALL_IMAGE_SIZEOF * 4, BALL_IMAGE_WH);
}

void DrawEmptyCell(uint8_t x, uint8_t y) {
    DrawEmptyCell1(CellAddress(x, y));
}

void DrawCell1(uint8_t *graphAddr, uint8_t color) {
    const uint8_t *image;
    if (color == 0)
        image = imgBoard + BALL_IMAGE_SIZEOF * 4;
    else
        image = imgBalls + (BALL_IMAGE_SIZEOF * 10) * (color - 1);
    DrawImage(graphAddr, image, BALL_IMAGE_WH);
}

void DrawCell(uint8_t x, uint8_t y, uint8_t color) {
    DrawCell1(CellAddress(x, y), color);
}

static uint8_t *removeAnimationImages[3] = {imgBalls + 5 * BALL_IMAGE_SIZEOF, imgBalls + 6 * BALL_IMAGE_SIZEOF,
                                            imgBalls + 7 * BALL_IMAGE_SIZEOF};

void DrawSpriteRemove(uint8_t x, uint8_t y, uint8_t c, uint8_t n) {
    DrawImage(CellAddress(x, y), removeAnimationImages[n] + (BALL_IMAGE_SIZEOF * 10) * (c - 1), BALL_IMAGE_WH);
}

void DrawSpriteNew1(uint8_t *graphAddress, uint8_t color, uint8_t phase) {
    DrawImage(graphAddress, imgBalls + (BALL_IMAGE_SIZEOF * 10) * (color - 1) + (4 - phase) * BALL_IMAGE_SIZEOF,
              BALL_IMAGE_WH);
}

void DrawSpriteNew(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    DrawSpriteNew1(CellAddress(x, y), color, phase);
}

void DrawSpriteStep(uint8_t x, uint8_t y, uint8_t color) {
    DrawImage(CellAddress(x, y), imgBoard + color * BALL_IMAGE_SIZEOF, BALL_IMAGE_WH);
    //    if (soundEnabled) {
    //        PLAY_SOUND_TICK
    //    }
}

static uint8_t *bouncingAnimation[6] = {imgBalls, imgBalls + BALL_IMAGE_SIZEOF * 9, imgBalls + BALL_IMAGE_SIZEOF * 9,
                                        imgBalls, imgBalls + BALL_IMAGE_SIZEOF * 8, imgBalls + BALL_IMAGE_SIZEOF * 8};

void DrawBouncingBall(uint8_t x, uint8_t y, uint8_t color, uint8_t phase) {
    uint8_t *graphAddress = CellAddress(x, y);
    DrawBall1(graphAddress, bouncingAnimation[phase], color);
}

static uint8_t *const helpCoords[NEW_BALL_COUNT] = {
    TEXTCOORDS(49, 23),
    TEXTCOORDS(53, 23),
    TEXTCOORDS(57, 23),
};

void DrawHelp(void) {
    uint8_t i;
    for (i = 0; i < NEW_BALL_COUNT; i++) {
        if (showHelp != 0) {
            DrawSpriteNew1(helpCoords[i], newBalls[i], 4 /* Размер шарика */);
        } else {
            DrawImage(helpCoords[i], imgBoard + imgBoardSize * 2 * 5, imgBoardSize);
        }
    }
}

// Курсор

static void DrawCursorXor(uint8_t *hl) {
}

void DrawCursor(void) {
    uint8_t *a = CellAddress(cursorX, cursorY);
    a[65] = '*';
    a[65 - 0x800] = COLOR_INK_LIGHT_YELLOW;
}

void ClearCursor(void) {
    DrawCell(cursorX, cursorY, game[cursorX][cursorY]);
}

void DrawButton(uint8_t *graphAddress, bool state) {
    uint8_t c = state ? COLOR_INK_LIGHT_CYAN | COLOR_PAPER_WHITE : COLOR_INK_GRAY | COLOR_PAPER_WHITE;
    graphAddress -= 0x800;
    while ((*graphAddress & COLOR_PAPER_WHITE) == COLOR_PAPER_WHITE) {
        *graphAddress = c;
        graphAddress++;
    }
}

void Intro(void) {
}

// Таблица рекордов

static const char hi0[] = "┌──────────────────────┐";
static const char hi1[] = "│   Лучшие результаты  │";
static const char hi2[] = "│                      │";
static const char hi3[] = "│    Нажмите пробел    │";
static const char hi4[] = "│    для продолжения   │";
static const char hi5[] = "│   Введите свое имя   │";
static const char hi6[] = "└──────────────────────┘";
static const unsigned HISCORE_X = (TEXT_WIDTH - sizeof(hi0) + 1) / 2;

void DrawHiScoresScreen1(uint8_t i, uint8_t pos) {
    struct HiScore *h = hiScores + i;
    for (; i < HISCORE_COUNT; ++i) {
        char text[sizeof(hi0) + 1];
        snprintf(text, sizeof(text), "│  %10s   %5u  │", h->name, h->score);
        text_color = (pos == i ? COLOR_INK_LIGHT_RED | COLOR_PAPER_BLUE : COLOR_INK_LIGHT_YELLOW | COLOR_PAPER_BLUE);
        DrawText(HISCORE_X, (HISCORE_Y + 3) + i, 0x7F, text);
        h++;
    }
}

void DrawHiScores(bool enterNameMode) {
    text_color = COLOR_INK_LIGHT_YELLOW | COLOR_PAPER_BLUE;
    DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y), 0x7F, hi0);
    DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y + 1), 0x7F, hi1);
    DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y + 2), 0x7F, hi2);
    if (enterNameMode != 0) {
        DrawHiScoresScreen1(0, HISCORE_COUNT - 1);
        text_color = COLOR_INK_LIGHT_YELLOW | COLOR_PAPER_BLUE;
        DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y + 4 + HISCORE_COUNT), 0x7F, hi2);
        DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y + 5 + HISCORE_COUNT), 0x7F, hi5);
    } else {
        DrawHiScoresScreen1(0, HISCORE_COUNT);
        text_color = COLOR_INK_LIGHT_YELLOW | COLOR_PAPER_BLUE;
        DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y + 4 + HISCORE_COUNT), 0x7F, hi3);
        DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y + 5 + HISCORE_COUNT), 0x7F, hi4);
    }
    DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y + 3 + HISCORE_COUNT), 0x7F, hi2);
    DrawText1(TEXTCOORDS(HISCORE_X, HISCORE_Y + 6 + HISCORE_COUNT), 0x7F, hi6);
}

void DrawHiScoresScreen(uint8_t pos) {
    DrawHiScoresScreen1(0, pos);
}

void DrawHiScoresLastLine(void) {
    DrawHiScoresScreen1(HISCORE_COUNT - 1, HISCORE_COUNT - 1);
}

void DrawScoreAndCreatures(void) {
    text_color = COLOR_INK_LIGHT_GREEN;
    char scoreText[UINT16_TO_STRING_SIZE + 1];
    Uint16ToString(scoreText, score, 10);
    DrawText1(TEXTCOORDS(52, 1), UINT16_TO_STRING_SIZE, scoreText);

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
        DrawImage(TEXTCOORDS(48, 7 - rightCreatureY), imgPlayer, imgPlayerSize);
        if (rightCreatureY == RIGHT_CREATURE_Y_MAX) {
            DrawImage(TEXTCOORDS(53, 4), imgPlayerWin, imgPlayerWinSize);
            DrawImage(TEXTCOORDS(4, 4), imgKingLose, imgKingLoseSize);
        }
    }
}

static void ClearAllScreen() {
    SetColor(COLOR_INK_LIGHT_WHITE);
    ClearScreen();
    memset((void *)0xE000, 0, 0x1000);
    memcpy((void *)0xE800, imgScreenC, sizeof(imgScreenC));
    memcpy((void *)0xE000, imgScreenA, sizeof(imgScreenA));
}

void DrawScreen(void) {
    // Рисуем фон
    ClearAllScreen();
    text_color = COLOR_INK_LIGHT_CYAN | COLOR_PAPER_WHITE;
    DrawText1(BUTTON_PATH, 16, "1 - Путь");
    DrawText1(BUTTON_SOUND, 16, "2 - Звук");
    DrawText1(BUTTON_HELP, 16, "3 - Подсказка");
    DrawText1(TEXTCOORDS(2, 25), 16, "4 - Рекорд");
    DrawText1(TEXTCOORDS(2, 26), 16, "5 - Новая игра");

    // Нужно перерисовать короля и претендента
    rightCreatureY = -1;

    // Ваш результат и изображение бойца слева
    DrawScoreAndCreatures();

    // Максимальный результат
    char buf[UINT16_TO_STRING_SIZE + 1];
    Uint16ToString(buf, hiScores[0].score, 10);
    DrawText1(TEXTCOORDS(7, 1), UINT16_TO_STRING_SIZE, buf);

    // Рисуем состояние кнопок
    if (showPath)
        DrawButton(BUTTON_PATH, true);
    if (soundEnabled)
        DrawButton(BUTTON_SOUND, true);
    if (showHelp) {
        DrawButton(BUTTON_HELP, true);
        DrawHelp();
    }

    // Игровое поле
    uint8_t x, y;
    for (y = 0; y < GAME_HEIGHT; y++)
        for (x = 0; x < GAME_WIDTH; x++)
            DrawCell(x, y, game[x][y]);
    DrawCursor();
}

uint8_t ReadKeyboard(bool noWait) {
    if (noWait)
        return ScanKey();
    return getchar();
}
