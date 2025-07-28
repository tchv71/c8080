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

#pragma once

#ifdef ARCH_CPM
int getchar(void) __link("cpm/getchar.c");
int putchar(int c) __link("cpm/putchar.c");
#else

#ifdef ARCH_86RK
int getchar(void) __link("86rk/getchar.c");
int putchar(int c) __link("86rk/putchar.c");
#endif

#ifdef ARCH_MICRO80_COLOR
int getchar(void) __link("micro80/getchar.c");
int putchar(int c) __link("micro80/putchar.c");
#endif

#ifdef ARCH_SPECIALIST
int getchar(void) __link("specialist/getchar.c");
int putchar(int c) __link("specialist/putchar.c");
#endif

#ifdef ARCH_ISKRA_1080_TARTU
int getchar(void) __link("iskra1080/getchar.c");
int putchar(int c) __link("iskra1080/putchar.c");
#endif

#endif
