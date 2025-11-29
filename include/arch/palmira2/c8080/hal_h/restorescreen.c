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
void __global mem_to_scr(void *, const void *, size_t);

void RestoreScreen(struct SavedScreen *s) {
    HideCursor();
    BIOS_COLOR = s->color;
    BIOS_CURSOR = s->cursor;
    mem_to_scr(SCREEN, s->screen, TEXT_WIDTH * TEXT_HEIGHT * 2);
    /* memcpy(SCREEN, s->screen + TEXT_WIDTH * TEXT_HEIGHT, TEXT_WIDTH * TEXT_HEIGHT); */
    if (s->cursor_visible)
        ShowCursor();
}

void __global mem_to_scr(void *, const void *, size_t) {
    asm {
__a_3_mem_to_scr=0
	ex   hl, de             ; de = size
__a_2_mem_to_scr=$
	ld   bc, 0              ; bc = source
__a_1_mem_to_scr=$
	ld   hl, 0              ; hl = destination
	ld   a, l
	out  (99h), a
	ld   a, h
	or   40h
	out  (99h), a
	inc  d                  ; enter loop
	xor  a
	or   e
	jp   z, mem_to_scr_2
mem_to_scr_1:
	ld   a, (bc)
	out  (98h), a
	;inc  hl
	inc  bc
	dec  e                  ; end loop
	jp   nz, mem_to_scr_1
mem_to_scr_2:
	dec  d
	jp   nz, mem_to_scr_1
    }
}


