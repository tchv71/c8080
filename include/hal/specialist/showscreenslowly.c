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

#include "specialist.h"

void ShowScreenSlowly(void) {
    asm {
        ld   b, 0
        ld   e, 0
ShowScreenSlowly_1:
        ld   a, e
        add  a
        add  a
        add  e
        inc  a
        ld   e, a
        ld   d, 48h
        ld   h, 90h
        ld   l, e
        ld   c, 48
ShowScreenSlowly_2:
        ld   a, (de)
        inc  d
        ld   (0F802h), a
        ld   a, (hl)
        ld   (hl), a
        inc  h
        dec  c
        jp   nz,  ShowScreenSlowly_2
        dec  b
        jp   nz,  ShowScreenSlowly_1
    }
}

