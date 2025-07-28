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

#include <stdio.h>
#include <stdint.h>

#ifdef ARCH_CPM
#include <cpmbios.h>

int getchar(void) {
    return cpmBiosConIn();
}
#else

#ifdef ARCH_86RK
int __global getchar(void) {
    asm {
        call 0F803h
        ld l, a
        ld h, 0
    }
}
#endif

#ifdef ARCH_SPECIALIST
int __global getchar(void) {
    asm {
        call 0C803h
        ld l, a
        ld h, 0
    }
}
#endif

#ifdef ARCH_ISKRA_1080_TARTU
int __global getchar(void) {
    asm {
        call 0C7F3h
        ld l, a
        ld h, 0
    }
}
#endif

#endif
