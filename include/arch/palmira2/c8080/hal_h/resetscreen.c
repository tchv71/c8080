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
#include <c8080/colors.h>
#include <string.h>

void ResetScreen(void) {
    scrset((void *)0x0, 0, 80 * 30);
}

void __global scrset(void *, uint8_t, size_t) {
    asm {
__a_3_scrset=0
        ex   hl, de             ; de = size
__a_1_scrset=$+1
        ld   hl, 0              ; hl = destination
    	ld   a, l
	out  (99h), a
	ld   a,h
	or   40h
	out  (99h), a
scrset_1:
        ld   a, 0
        out  (98h), a
__a_2_scrset=$+1
        ld   a, 0               ; bc = byte
        out  (98h), a
        ;inc  hl
        inc  bc
        dec  de                 ; end loop
        ld   a, d
        or   e
        jp   nz, scrset_1
        ;ld   hl, (__a_1_scrset) ; return destination
    }
}

