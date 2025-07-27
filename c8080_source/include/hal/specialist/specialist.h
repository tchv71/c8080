/* c8080 stdlib
 * Copyright (c) 2022 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

void Reboot(void) __address(0xC000);
void ShowScreenSlowly(void) __link("showscreenslowly.c");
void __global DrawImageTile(void *tile, const void *image, uint16_t width_height) __link("drawimagetile.c");
void __global DrawImageTileMono(void* tile, const void* image, uint16_t width_height) __link("drawimagetilemono.c");
void __global Sound(uint8_t period, uint16_t count)__link("sound.c");

// Color codes

static const uint8_t COLOR_INK_WHITE  = 0x00;
static const uint8_t COLOR_INK_YELLOW = 0x10;
static const uint8_t COLOR_INK_VIOLET = 0x40;
static const uint8_t COLOR_INK_RED    = 0x50;
static const uint8_t COLOR_INK_CYAN   = 0x80;
static const uint8_t COLOR_INK_GREEN  = 0x90;
static const uint8_t COLOR_INK_BLUE   = 0xC0;
static const uint8_t COLOR_INK_BLACK  = 0xD0;

