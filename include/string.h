/*
 * c8080 stdlib
 * Copyright (c) 2022 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <c8080/c8080.h>

int8_t __global memcmp(const void *buffer1, const void *buffer2, size_t size) __link("string/memcmp.c");
void *__global memcpy(void *destination, const void *source, size_t size) __link("string/memcpy.c");
void *__global memmove(void *destination, const void *source, size_t size) __link("string/memmove.c");
void *__global memset(void *destination, uint8_t byte, size_t size) __link("string/memset.c");
void __global memswap(void *buffer1, void *buffer2, size_t size) __link("string/memswap.c");
const char *__global strchr(const char *string, char byte) __link("string/strchr.c");
int8_t __global strcmp(const char *string1, const char *string2) __link("string/strcmp.c");
char *__global strcpy(char *destination, const char *source) __link("string/strcpy.c");
size_t __global strlen(const char *string) __link("string/strlen.c");
