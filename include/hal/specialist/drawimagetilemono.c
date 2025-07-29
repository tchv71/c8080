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

#include "hal.h"

void __global DrawImageTileMono(void *tile, const void *image, uint16_t width_height) {
    asm {
__a_3_drawimagetilemono=0
        ld   bc, hl
__a_1_drawimagetilemono=$+1
        ld   de, 0
__a_2_drawimagetilemono=$+1
        ld   hl, 0
DrawImageTileMono_1:
        push de
        push bc
DrawImageTileMono_2:
        ld   a, (hl)
        inc  hl
        ld   (de), a
        inc  de
        dec  b
        jp   nz, DrawImageTileMono_2
        pop  bc
        pop  de
        inc  d
        dec  c
        jp   nz, DrawImageTileMono_1
    }
}
