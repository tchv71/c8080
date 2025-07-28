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
#include <codepage/koi7.h>

#define TILE(X, Y) (uint8_t *)(0xE800 + (X) + (Y)*64)

void ClearScreen(void) __link("clearscreen.c");
void MoveCursorHome(void) __link("movecursorhome.c");
void ShowCursor(void) __link("showcursor.c");
void HideCursor(void) __link("hidecursor.c");
uint8_t ScanKey(void) __address(0xF81B);

// Key codes

static const uint8_t KEY_UP = 0x19;
static const uint8_t KEY_LEFT = 0x08;
static const uint8_t KEY_RIGHT = 0x18;
static const uint8_t KEY_DOWN = 0x1A;
static const uint8_t KEY_ENTER = 0x0D;
static const uint8_t KEY_BACKSPACE = 0x7F;

// Other codes

static const uint8_t TEXT_WIDTH = 64;
static const uint8_t TEXT_HEIGHT = 25;
