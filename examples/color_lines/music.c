/*
 * Game "Color Lines" for 8 bit computers
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

#include "music.h"
#include <c8080/hal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROUND_DIV(A, B) (((A) + (B) / 2) / (B))

#define BASE_PERIOD 65536
#define BASE_LENGTH 6000

#define C ROUND_DIV(BASE_PERIOD, 262)
#define D ROUND_DIV(BASE_PERIOD, 294)
#define E ROUND_DIV(BASE_PERIOD, 330)
#define F ROUND_DIV(BASE_PERIOD, 349)
#define G ROUND_DIV(BASE_PERIOD, 392)
#define A ROUND_DIV(BASE_PERIOD, 440)
#define C2 ROUND_DIV(BASE_PERIOD, 523)

#define ITEM(N, X) (N), (uint32_t)(X)*BASE_LENGTH / (N)

static const uint16_t music[] = {
    ITEM(F, 8), ITEM(A, 4),  ITEM(C2, 4), ITEM(A, 4), ITEM(F, 8), ITEM(C, 4),  ITEM(D, 4),  ITEM(E, 4), ITEM(F, 8),
    ITEM(A, 4), ITEM(C2, 4), ITEM(A, 4),  ITEM(G, 8), ITEM(C, 4), ITEM(D, 4),  ITEM(E, 4),  ITEM(E, 8), ITEM(C, 4),
    ITEM(D, 4), ITEM(E, 4),  ITEM(F, 16), ITEM(F, 8), ITEM(A, 4), ITEM(C2, 4), ITEM(A, 4),  ITEM(G, 8), ITEM(C, 4),
    ITEM(D, 4), ITEM(E, 4),  ITEM(E, 8),  ITEM(C, 4), ITEM(D, 4), ITEM(E, 4),  ITEM(F, 16), 0,
};

void PlayMusic(void) {
    while (InKey() != 0xFF)
        rand();
    const uint16_t *i = music;
    for (;;) {
        uint8_t period = *i;
        ++i;
        if (period == 0) {
            while (InKey() == 0xFF)
                rand();
            break;
        }
        if (InKey() != 0xFF)
            break;
        Sound(period, *i);
        ++i;
        rand();
    }
}
