/*
 * Draw text function for 8-bit Computer Specialist
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

#pragma once

#include <stdint.h>

#define DRAWTEXTARGS(XX, YY) ((uint8_t *)0x9000 + (YY) + (((XX) >> 2) * 256)), (XX)&3

void DrawTextXor(void);
void DrawTextInverse(void);
void DrawTextNormal(void);
void DrawText(uint8_t *tile, uint8_t pixelOffset, const char *text);
void DrawTextXY(uint8_t x, uint8_t y, const char *text);
