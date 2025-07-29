/*
 * Fillrect function for 8-bit Computer Specialist
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

#include <hal/hal.h>

#define FILLRECTARGS(X0, Y, X1, Y1)                                                                         \
    (uint8_t *)((((X0) / 8) * 256) + (Y) + 0x9000), ((X1) + 1) / 8 - (X0 / 8), (0xFF >> ((uint8_t)(X0)&7)), \
        (0xFF >> ((uint8_t)((X1) + 1) & 7)) ^ 0xFF, (Y1) - (Y) + 1

void FillRectOver(void);
void FillRectInverse(void);
void FillRectTrue(void);
void FillRect(uint8_t *tile, uint16_t centerSize, uint8_t leftMask, uint8_t rightMask, uint8_t height);
void FillRectXY(uint16_t x, uint8_t y, uint16_t x1, uint8_t y1);
