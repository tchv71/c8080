// c8080 stdlib
// Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>

static uint8_t *const SCREEN = (uint8_t *)0x9000;
static const uint16_t SCREEN_SIZE = 0x3000;
#define TILE(X, Y) (SCREEN + (Y) + ((X) << 8))

// Bios

void __global Reboot(void) __address(0xC000);
uint8_t __global GetCharBios(void) __address(0xC803);
uint8_t __global GetTape(uint8_t sync_flag) __address(0xC806);  // 0xFF - with sync byte search, 0x08 - without
void __global PutCharBios(uint8_t c) __link("putcharbios.c");   // 0xC809
// TODO:
void __global PutHex(uint8_t value) __address(0xC815);
void __global PutString(const char *) __address(0xC818);
uint16_t __global GetCursorPos(void) __address(0xC81E);  // Н - row, L - column
// TODO:
uint16_t GetRamTop(void) __address(0xC830);
void SetRamTop(uint16_t) __address(0xC833);

#define SET_COLOR(C)                     \
    do {                                 \
        *(uint8_t *)0xFFFE = (C) & 0xD0; \
    } while (0)

// HAL

void ShowScreenSlowly(uint8_t high_byte_of_addr) __link("showscreenslowly.c");
void __global DrawImageTile(void *tile, const void *image, uint16_t width_height) __link("drawimagetile.c");
void __global ChangeTileColor(void *tile, uint8_t width, uint8_t height) __link("changetilecolor.c");
void __global DrawImageTileMono(void *tile, const void *image, uint16_t width_height) __link("drawimagetilemono.c");
void __global Sound(uint8_t period, uint16_t count) __link("sound.c");

// Color codes

static const uint8_t COLOR_INK_WHITE = 0x00;
static const uint8_t COLOR_INK_YELLOW = 0x10;
static const uint8_t COLOR_INK_VIOLET = 0x40;
static const uint8_t COLOR_INK_RED = 0x50;
static const uint8_t COLOR_INK_CYAN = 0x80;
static const uint8_t COLOR_INK_GREEN = 0x90;
static const uint8_t COLOR_INK_BLUE = 0xC0;
static const uint8_t COLOR_INK_BLACK = 0xD0;

// Key codes

static const uint8_t KEY_BACKSPACE = 0x08;
static const uint8_t KEY_TAB = 0x09;
static const uint8_t KEY_ENTER = 0x0D;
static const uint8_t KEY_UP = 0x17;
static const uint8_t KEY_RIGHT = 0x18;
static const uint8_t KEY_LEFT = 0x19;
static const uint8_t KEY_DOWN = 0x1A;
static const uint8_t KEY_ESC = 0x1B;
static const uint8_t KEY_F1 = 0xF3;
static const uint8_t KEY_F2 = 0xF4;
static const uint8_t KEY_F3 = 0xF5;
static const uint8_t KEY_F4 = 0xF6;
static const uint8_t KEY_F5 = 0xF7;
static const uint8_t KEY_F6 = 0xF8;
static const uint8_t KEY_F7 = 0xF9;
static const uint8_t KEY_F8 = 0xFA;
static const uint8_t KEY_F9 = 0xFB;
static const uint8_t KEY_F10 = 0xFC;
static const uint8_t KEY_F11 = 0xFD;
static const uint8_t KEY_F12 = 0xFE;
