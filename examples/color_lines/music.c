#include "music.h"
#include <hal/hal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hal.h"

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

static const uint16_t music[] = {ITEM(F, 8), ITEM(A, 4), ITEM(C2, 4), ITEM(A, 4),  ITEM(F, 8), ITEM(C, 4),  ITEM(D, 4),
                                 ITEM(E, 4), ITEM(F, 8), ITEM(A, 4),  ITEM(C2, 4), ITEM(A, 4), ITEM(G, 8),  ITEM(C, 4),
                                 ITEM(D, 4), ITEM(E, 4), ITEM(E, 8),  ITEM(C, 4),  ITEM(D, 4), ITEM(E, 4),  ITEM(F, 16),
                                 ITEM(F, 8), ITEM(A, 4), ITEM(C2, 4), ITEM(A, 4),  ITEM(G, 8), ITEM(C, 4),  ITEM(D, 4),
                                 ITEM(E, 4), ITEM(E, 8), ITEM(C, 4),  ITEM(D, 4),  ITEM(E, 4), ITEM(F, 16), 0};

void PlayMusic(void) {
    const uint16_t *i = music;
    for (;;) {
        uint8_t s = *i;
        ++i;
        if (s == 0) {
            while (!ReadKeyboard(true))
                ;
            break;
        }
        if (ReadKeyboard(true))
            break;
        Sound(s, *i);
        ++i;
        rand();
    }
}
