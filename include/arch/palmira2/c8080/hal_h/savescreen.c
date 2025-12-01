/*
 * c8080 stdlib
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <c8080/hal.h>
#include <c8080/console.h>
#include <string.h>

extern uint8_t BIOS_COLOR __address(0xF759);
extern uint16_t BIOS_CURSOR __address(0xF75A);
extern uint8_t BIOS_CURSOR_VISIBLE __address(0xF75E);

void __global scr_to_mem(void *, const void *, size_t);


void SaveScreen(struct SavedScreen *s) {
    s->color = BIOS_COLOR;
    s->cursor = BIOS_CURSOR;
    s->cursor_visible = BIOS_CURSOR_VISIBLE;
    HideCursor();
    scr_to_mem(s->screen, SCREEN, TEXT_WIDTH * TEXT_HEIGHT * 2);
    memcpy(s->context, &BIOS_CURSOR_VISIBLE + 2, sizeof(s->context));
    /* memcpy(s->screen + TEXT_WIDTH * TEXT_HEIGHT, SCREEN, TEXT_WIDTH * TEXT_HEIGHT); */
    if (s->cursor_visible)
        ShowCursor();
}

void __global scr_to_mem(void *, const void *, size_t) {
    asm {
__a_3_scr_to_mem=0
        ex   hl, de             ; de = size
__a_2_scr_to_mem=$+1
        ld   bc, 0              ; bc = source
__a_1_scr_to_mem=$+1
        ld   hl, 0              ; hl = destination
        inc  d                  ; enter loop
        xor  a
        or   e
        jp   z, scr_to_mem_2
        ld   a,c
        out  (99h), a
        ld   a,b
        out  (99h), a
scr_to_mem_1:
        ;ld   a, (bc)
        in   a, (98h)
        ld   (hl), a
        inc  hl
        ;inc  bc
        dec  e                  ; end loop
        jp   nz, scr_to_mem_1
scr_to_mem_2:
        dec  d
        jp   nz, scr_to_mem_1
    }
}

