/*
 * Convert PNG files to Specialist resource file
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

#pragma once

#include <png.h>
#include <stdint.h>

class Png {
protected:
    bool loaded = false;
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t color_type = 0;
    uint8_t bit_depth = 0;
    int number_of_passes = 0;
    png_bytep *row_pointers = nullptr;

public:
    bool load(const char *file_name);
    bool save(const char *file_name) const;
    uint32_t getPixel(unsigned x, unsigned y) const;
    void setPixel(unsigned x, unsigned y, uint32_t c);
    inline uint32_t getWidth() const {
        return width;
    };
    inline uint32_t getHeight() const {
        return height;
    };
    ~Png();
};
