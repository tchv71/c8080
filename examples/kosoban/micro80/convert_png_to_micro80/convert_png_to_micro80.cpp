/*
 * Convert PNG files for 8-bit computer Micro 80
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

#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <inttypes.h>
#include "png.h"
#include "megalz.h"

static std::string cFile, hFile;

static std::string MakeCSourceArray(const uint8_t *data, size_t size, const char *tab = "   ") {
    std::string result;
    uint8_t numbers_in_line = 0;
    for (const uint8_t *dataEnd = data + size; data != dataEnd; data++) {
        uint8_t b = *data;
        if (numbers_in_line == 16) {
            numbers_in_line = 0;
            result += "\n";
        }
        if (numbers_in_line == 0)
            result += tab;
        char buffer[16];
        (void)snprintf(buffer, sizeof(buffer), " 0x%02X,", b);
        result += buffer;
        numbers_in_line++;
    }
    result += "\n";
    return result;
}

static std::string MakeCSourceArray2(const uint8_t *data, size_t size, size_t imageCount, size_t imageSize) {
    if (imageCount * imageSize != size)
        throw std::runtime_error("MakeCSourceArray2 internal error");

    std::string result;
    for (uint32_t i = 0; i < imageCount; i++) {
        result += "    {\n";
        result += MakeCSourceArray(data + i * imageSize, imageSize, "        ");
        result += "    },\n";
    }
    return result;
}

const uint32_t palette[] = {
    0x000000,
    0xAA0000, // blue
    0x00AA00, // green
    0xAAAA00, // cyan
    0x0000AA, // red
    0xAA00AA, // magenta
    0x00AAAA, // yellow
    0xAAAAAA, // white
    0x555555, // gray
    0xFF5555, // light blue
    0x55FF55, // light green
    0xFFFF55, // light cyan
    0x5555FF, // light red
    0xFF55FF, // light magenta
    0x55FFFF, // light yellow
    0xFFFFFF, // light white
};

static uint32_t FindColor(uint32_t color) {
    uint32_t bestIndex = 0;
    uint32_t bestDist = UINT32_MAX;
    for (uint8_t i = 0; i < std::size(palette); i++) {
        int32_t dr = int32_t(palette[i] & 0xFF) - int32_t(color & 0xFF);
        int32_t dg = int32_t((palette[i] >> 8) & 0xFF) - int32_t((color >> 8) & 0xFF);
        int32_t db = int32_t((palette[i] >> 16) & 0xFF) - int32_t((color >> 16) & 0xFF);
        int32_t dist = dr * dr + dg * dg + db * db;
        if (dist < bestDist) {
            bestDist = dist;
            bestIndex = i;
        }
    }
    return bestIndex;
}

static const unsigned FW = 8;
static const unsigned FH = 10;

struct Id {
    uint64_t data[2] = {};

    void SetLine(unsigned y, uint8_t d) {
        data[y / 8] |= uint64_t(d) << ((y % 8) * 8);
    }

    bool operator == (const Id& b) {
        return data[0] == b.data[0] && data[1] == b.data[1];
    }
};

Id chargen[256];

static void Encode4(Png &png, unsigned x, unsigned y, uint8_t& out_char, uint8_t& out_attrib)
{
    unsigned ink = UINT_MAX;
    unsigned paper = UINT_MAX;
    Id id;
    for (unsigned iy = 0; iy < FH; iy++) {
        uint8_t d = 0;
        for (unsigned ix = 0; ix < FW; ix++) {
            const uint32_t c = FindColor(png.getPixel(x + ix, y + iy));
            if (ink == c || ink == UINT_MAX) {
                ink = c;
                d |= (0x80 >> ix);
            } else if (paper == c || paper == UINT_MAX) {
                paper = c;
            } else
                throw std::runtime_error("Incorrect colors at " + std::to_string(x) + " " + std::to_string(y));
        }
        id.SetLine(iy, d);
    }

    if (paper == UINT_MAX) {
        if (ink < 8) {
            paper = ink;
            out_char = 0;
        } else {
            paper = 0;
            out_char = 0x17;
        }
    } else {
        if (ink < paper) {
            std::swap(ink, paper);
            id.data[0] ^= UINT64_MAX;
            id.data[1] ^= 0xFFFF;
        }

        if (paper >= 8) {
            throw std::runtime_error("Incorrect colors at " + std::to_string(x)
                                     + " " + std::to_string(y) + ", all light, ink "
                                     + std::to_string(ink) + ", paper " + std::to_string(paper));
        }

        bool found = false;
        for (unsigned i = 0; i < 256; i++) {
            if (chargen[i] == id) {
                out_char = i;
                found = true;
                break;
            }
        }
        if (!found && ink < 8) {
            std::swap(ink, paper);
            id.data[0] ^= UINT64_MAX;
            id.data[1] ^= 0xFFFF;
            for (unsigned i = 0; i < 256; i++) {
                if (chargen[i] == id) {
                    out_char = i;
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            out_char = 0;
            std::cerr << "Incorrect char at " << x << " " << y << std::endl;
            fprintf(stderr, "%04" PRIu64 "%016" PRIu64 " ink %u paper %u\n", id.data[1], id.data[0], ink, paper);
        }
    }

//    if (x + y == 0)
//        fprintf(stderr, "%016lx %016lx %02X %u %u\n", id.data[0], id.data[1], out_char, ink, paper);

    out_attrib = (ink & 0x0F) | ((paper & 7) << 4);
}

static void ConvertImage(const std::string &inputFileName, const std::string &id, uint32_t cw, uint32_t ch,
                         bool modeColors, bool modePack) {
    Png png;
    if (!png.load(inputFileName.c_str()))
        throw std::runtime_error("Can't open file " + inputFileName);

    cw *= FW;
    ch *= FH;

    if (png.getWidth() % FW || png.getHeight() % FH)
        throw std::runtime_error("The width and height must be a multiple of char");

    uint32_t imageSize;
    if (cw != 0 && ch != 0) {
        imageSize = (cw / FW) * (ch / FH);
        if (modeColors && !modePack)
            imageSize *= 2;
        if (!modePack)
            imageSize += 2; // w, h
    } else {
        imageSize = 0;
        cw = png.getWidth();
        ch = png.getHeight();
    }

    if (png.getWidth() % cw || png.getHeight() % ch)
        throw std::runtime_error("The width and height must be a multiple of item");

    std::vector<uint8_t> data, data2;
    for (uint32_t iy = 0; iy < png.getHeight(); iy += ch) {
        for (uint32_t ix = 0; ix < png.getWidth(); ix += cw) {
            if (!modePack) {
                data.push_back(cw / FW);
                data.push_back(ch / FH);
            }
            for (uint32_t y = 0; y < ch; y += FH) {
                for (uint32_t x = 0; x < cw; x += FW) {
                    uint8_t code, attrib;
                    Encode4(png, x + ix, y + iy, code, attrib);                        
                    if (modeColors) {
                        if (modePack)
                            data2.push_back(attrib);
                        else
                            data.push_back(attrib);
                    }
                    data.push_back(code);
                }
            }
        }
    }

    if (modePack) {
        PackMegalz(data);
        PackMegalz(data2);
        data.insert(data.end(), data2.begin(), data2.end());
        data2.clear();
    }

    hFile +=
        "\n"
        "static const unsigned " +
        id + "Width = " + std::to_string(cw / FW) + ";\n" + "static const unsigned " + id +
        "Height = " + std::to_string(ch / FH) + ";\n" + "static const unsigned " + id + "Size = " + id + "Width + (" +
        id + "Height << 8);\n";

    if (imageSize == 0) {
        hFile += "extern uint8_t " + id + "[" + std::to_string(data.size()) + "];\n";

        cFile +=
            "\n"
            "uint8_t " +
            id + "[" + std::to_string(data.size()) + "] = {\n" + MakeCSourceArray(data.data(), data.size()) + "};\n";
    } else {
        uint32_t imageCount = data.size() / imageSize;
        hFile += "extern uint8_t " + id + "[" + std::to_string(imageCount) + "][" + std::to_string(imageSize) + "];\n";

        cFile +=
            "\n"
            "uint8_t " +
            id + "[" + std::to_string(imageCount) + "][" + std::to_string(imageSize) + "] = {\n" +
            MakeCSourceArray2(data.data(), data.size(), imageCount, imageSize) + "};\n";
    }
}

static uint32_t StrToUint32(const char text[], int base = 0) {
    char *end = nullptr;
    errno = 0;
    const unsigned long value = strtoul(text, &end, base);
    if (end[0] != '\0' || errno != 0)
        throw std::runtime_error(std::string("Can't convert \"") + text + "\" to number");
    return value;
}

static std::string GetElement(std::string &s) {
    const auto end = s.find('.');
    std::string e;
    if (end == std::string::npos) {
        e = s;
        s.clear();
    } else {
        e.assign(s.data(), end);
        s.erase(0u, end + 1);
    }
    return e;
}

static void MakeFont(const char* path) {
    std::ifstream s((std::string(path) + "/font.bin").c_str(), std::ifstream::in | std::ifstream::binary);
    char data[8192];
    s.read(data, sizeof(data));

    if (!s) {
        std::cerr << "font.bin not found" << std::endl;
        return;
    }

    Png png;
    if (!png.create(16 * 9, 16 * 11))
        return;

    for (unsigned iy = 0; iy < 16; iy++) {
        for (unsigned ix = 0; ix < 16; ix++) {
            uint8_t c = (ix + iy * 16);
            unsigned gx = ix * 9, gy = iy * 11;
            struct Id id;
            for (unsigned y = 0; y < 10; y++) {
                uint8_t d = data[c * 8 + (y & 7) + ((y & 8) ? 2048 : 0)];
                id.SetLine(y, d);
                for (unsigned x = 0; x < 8; x++) {
                    png.setPixel(gx + x, gy + y, (d & (0x80 >> x)) ? 0xFFFFFF : 0);
                }
            }
            id.data[0] ^= UINT64_MAX;
            id.data[1] ^= 0xFFFF;
            chargen[c] = id;
        }
    }

    png.save((std::string(path) + "/font.png").c_str());
}

int main(int argc, char **argv) {
    try {
        std::cout << "Micro 80 graph convertor (c) 2025 Alemorf" << std::endl;

        if (argc != 2) {
            std::cerr << "Using: " << argv[0] << " path" << std::endl;
            return 1;
        }

        MakeFont(argv[1]);

        hFile =
            "// Autogenerated\n\n"
            "#include <stdint.h>\n";

        cFile =
            "// Autogenerated\n"
            "#include \"resources.h\"\n\n";

        for (const auto &entry : std::filesystem::directory_iterator(argv[1])) {
            if (entry.path().extension() == ".png") {
                std::string fileName = entry.path().stem();
                if (fileName == "font")
                    continue;
                std::cout << "Process file " << entry.path().string() << std::endl;
                std::string id = GetElement(fileName);
                std::string algorithm = GetElement(fileName);
                uint32_t cw = 0, ch = 0;
                if (!fileName.empty()) {
                    cw = StrToUint32(GetElement(fileName).c_str());
                    ch = StrToUint32(fileName.c_str());
                }

                if (algorithm == "mono") {
                    ConvertImage(entry.path(), id, cw, ch, false, false);
                } else if (algorithm == "color") {
                    ConvertImage(entry.path(), id, cw, ch, true, false);
                } else if (algorithm == "screen") {
                    ConvertImage(entry.path(), id, cw, ch, true, true);
                } else {
                    throw std::runtime_error("Unsupported algorithm '" + algorithm + "'");
                }
            }
        }

        std::string cFileName = std::string(argv[1]) + "/resources.c";
        std::ofstream cFileStream(cFileName);
        cFileStream << cFile;
        if (!cFileStream)
            throw std::runtime_error("Can't create file " + cFileName);

        std::string hFileName = std::string(argv[1]) + "/resources.h";
        std::ofstream hFileStream(hFileName);
        hFileStream<< hFile;
        if (!hFileStream)
            throw std::runtime_error("Can't create file " + hFileName);

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
