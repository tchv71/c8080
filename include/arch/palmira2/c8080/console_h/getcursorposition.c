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

#include <c8080/console.h>
#include <cpm.h>

uint16_t GetCursorPosition(void)
{
    (void)CpmConsoleRead;
    asm
    {
	;CpmConsoleWriteString("\033Z$");
	;return ((uint16_t) CpmConsoleRead()) << 8 | CpmConsoleRead();
	ld   hl, getcursorposition_data
	call __puts;0F818h
	call cpmconsoleread
	ld   l,a
	push hl
	call cpmconsoleread
	pop  hl
	ld   h,a
	ret
getcursorposition_data:
	.db 1Bh, 'Z', 0

    }
}
