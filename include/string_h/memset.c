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

#include <string.h>

void *__global memset(void *destination, uint8_t byte, size_t size) {
    (void)destination;
    (void)byte;
    (void)size;
    asm {
        ex hl, de             ; de = size
        ld hl, (__a_1_memset) ; hl = destination
        inc d                 ; enter loop
        xor a
        or e
        ld a, (__a_2_memset)  ; bc = byte
        jp z, memset_2
memset_1:
        ld (hl), a
        inc hl
        inc bc
        dec e               ; end loop
        jp nz, memset_1
memset_2:
        dec d
        jp nz, memset_1
        ld hl, (__a_1_memset) ; return destination
    }
}
