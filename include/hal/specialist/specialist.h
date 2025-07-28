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

#define __C8080_CPU_FREQ 2000000

// Bios

void __global Reboot(void) __address(0xC000);
uint8_t __global GetChar(void) __address(0xC803);
uint8_t __global GetTape(uint8_t sync_flag) __address(0xC806);  // 0xFF - with sync byte search, 0x08 - without
void __global PutChar(uint8_t c) __link("putchar.c");           // 0xC809
// TODO:
void __global PutHex(uint8_t value) __address(0xC815);
void __global PutString(const char *) __address(0xC818);
uint16_t __global GetCursorPos(void) __address(0xC81E);  // Н - row, L - column
// TODO:
uint16_t GetRamTop(void) __address(0xC830);
void SetRamTop(uint16_t) __address(0xC833);

// HAL

void ShowScreenSlowly(uint8_t high_byte_of_addr) __link("showscreenslowly.c");
void __global DrawImageTile(void *tile, const void *image, uint16_t width_height) __link("drawimagetile.c");
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
