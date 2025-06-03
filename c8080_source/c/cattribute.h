/*
 * c8080 compiler
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include <string>
#include <stdint.h>

// Example: __link(0xF800) void reboot(void);
// TODO: __address

struct CAddressAttribute {
    bool exists{};
    uint64_t value{};
};

// Example: __link("stdio/snprintf.c") int snprintf(char *buffer, size_t bufer_size, const char *format, ...);
// TODO: __include

struct CIncludeAttribute {
    bool exists{};
    std::string base_name;
    std::string name_for_path;
};
