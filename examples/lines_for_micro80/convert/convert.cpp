/* 
 * Game "Color Lines" for Micro 80
 * Copyright (c) 2025 Aleksey Morozov
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

#include "png.h"
#include "fs_tools.h"
#include <algorithm>
#include <vector>
#include <map>
#include <string.h>
#include <string>
#include <iostream>
#include <assert.h>

static std::string truncFileExt(const char* str) {
    char* ext = strrchr((char*)str, '.');
    if (!ext) return str;
    if (strchr(ext, '/')) return str;
    return std::string(str, ext - str);
}

static std::string IdFromFileName(const char fullName[]) {
    assert(fullName != nullptr);
    const char* name = strrchr(fullName, '/');
    if (name == nullptr)
        name = fullName;
    else
        name++;
    const char* end = strchr(name, '.');
    return end == nullptr ? name : std::string(name, end - name);
}

static std::string RemoveExtension(const char fullName[]) {
    assert(fullName != nullptr);
    const char* ext = strrchr(fullName, '.');
    if (ext == nullptr) return fullName;
    const char* folder = strrchr(fullName, '/');
    if (folder != nullptr && folder > ext) return fullName;
    return std::string(fullName, ext - fullName);
}

static std::string MakeCSourceArray(const std::vector<uint8_t>& comressed) {
    std::string result;
    uint8_t l = 0;
    for (uint8_t b : comressed) {
        if (l == 16) {
            l = 0;
            result += "\n";
        }
        if (l == 0) result += "   ";
        char buf[16];
        (void)snprintf(buf, sizeof(buf), " 0x%02X,", b);
        result += buf;
        l++;
    }
    result += "\n";
    return result;
}

const uint32_t palette[] = {
    0x000000,
    0x0000AA, // red
    0x00AA00, // green
    0x00AAAA, // yellow
    0xAA0000, // blue
    0xAA00AA, // magenta
    0xAAAA00, // cyan
    0xAAAAAA, // white
    0x555555, // gray
    0x5555FF, // light red
    0x55FF55, // light green
    0x55FFFF, // light yellow
    0xFF5555, // light blue
    0xFF55FF, // light magenta
    0xFFFF55, // light cyan
    0xFFFFFF, // light white
};

static uint32_t FindColor(uint32_t color) {
    assert(palette != nullptr);
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
#if 0
bool convert(const char* inputFileName, const uint32_t palette[4]) {
    Png png;
    if (!png.load(inputFileName)) return false;

    std::vector<uint8_t> dataA, dataB;

    for (uint32_t x = png.getWidth(); x >= 8; x -= 8) {
        for (uint32_t y = png.getHeight(); y != 0; y--) {
            uint8_t a = 0, b = 0;
            for (uint32_t p = 0; p < 8; p++) {
                uint32_t color = png.getPixel(x - 8 + p, y - 1);
                int colorIndex = FindColor(palette, color);
                if ((colorIndex & 1) != 0) {
                    a |= (0x80 >> p);
                }
                if ((colorIndex & 2) != 0) {
                    b |= (0x80 >> p);
                }
            }
            dataA.push_back(a);
            dataB.push_back(b);
        }
    }

    std::vector<uint8_t> comressedA;
    PackMegalz(dataA.data(), dataA.size(), comressedA);
    std::vector<uint8_t> comressedB;
    PackMegalz(dataB.data(), dataB.size(), comressedB);
    comressedA.insert(comressedA.begin(), comressedB.begin(), comressedB.end());

    std::string id = IdFromFileName(inputFileName);

    std::string hFile =
        "#include <stdint.h>\n"
        "\n"
        "extern uint8_t " +
        id + "[" + std::to_string(comressedA.size()) + "];\n";

    std::string cFile = "#include \"" + RemoveExtension(basename(inputFileName)) +
                        ".h\"\n"
                        "\n"
                        "uint8_t " +
                        id + "[" + std::to_string(comressedA.size()) + "] = {\n" + MakeCSourceArray(comressedA) +
                        "};\n";

    const std::string nameWoExtension = RemoveExtension(inputFileName);

    FsTools::SaveFile(nameWoExtension + ".c", cFile.data(), cFile.size());
    FsTools::SaveFile(nameWoExtension + ".h", hFile.data(), hFile.size());
    return true;
}
#endif

void Encode4(Png &png, unsigned x, unsigned y, uint8_t& out_char, uint8_t& out_attrib)
{
    unsigned c1 = FindColor(png.getPixel(x + 0, y + 0));
    unsigned c2 = FindColor(png.getPixel(x + 1, y + 0));
    unsigned c3 = FindColor(png.getPixel(x + 1, y + 1));
    unsigned c4 = FindColor(png.getPixel(x + 0, y + 1));

    unsigned ink = c1;
    unsigned paper = c1;
    if (ink != c2)
        paper = c2;
    if (ink != c3) {
        if (paper != c1 && paper != c3)
            throw std::runtime_error("Incorrect colors at " + std::to_string(x) + " " + std::to_string(y));
        paper = c3;
    }
    if (ink != c4) {
        if (paper != c1 && paper != c4)
            throw std::runtime_error("Incorrect colors at " + std::to_string(x) + " " + std::to_string(y));
        paper = c4;
    }

    if (ink == paper) {
        paper = 0;
        out_char = 0x17;
    } else {
        if (ink < paper)
            std::swap(ink, paper);

        if (paper >= 8)
            throw std::runtime_error("Incorrect colors at " + std::to_string(x)
                                     + " " + std::to_string(y) + ", all light, ink "
                                     + std::to_string(ink) + ", paper " + std::to_string(paper));

        out_char = 0;
        if (c1 == ink) out_char |= 0x01;
        if (c2 == ink) out_char |= 0x02;
        if (c3 == ink) out_char |= 0x04;
        if (c4 == ink) out_char |= 0x10;
    }

    out_attrib = (ink & 7) | ((paper & 7) << 3) | ((ink & 8) << 3);
}

static bool convert(const char* inputFileName) {
    Png png;
    if (!png.load(inputFileName)) return false;

    std::vector<uint8_t> data1;
    std::vector<uint8_t> data2;
    for (uint32_t y = 0; y + 2 <= png.getHeight(); y += 2) {
        for (uint32_t x = 0; x + 2 <= png.getWidth(); x += 2) {
            uint8_t code, attrib;
            Encode4(png, x, y, code, attrib);
            data1.push_back(code);
            data2.push_back(attrib);
        }
    }

    std::string id = IdFromFileName(inputFileName);

    std::string hFile =
        "#include <stdint.h>\n"
        "\n"
        "static const unsigned " + id + "Width = " + std::to_string(png.getWidth() / 2) + ";\n" +
        "static const unsigned " + id + "Height = " + std::to_string(png.getHeight() / 2) + ";\n" +
        "static const unsigned " + id + "Size = " + id + "Width + (" + id + "Height << 8);\n" +
        "\n" +
        "extern uint8_t " +
        id + "C[" + std::to_string(data1.size()) + "];\n" +
        "\n"
        "extern uint8_t " +
        id + "A[" + std::to_string(data2.size()) + "];\n";

    std::string cFile = "#include \"" + RemoveExtension(basename(inputFileName)) +
                        ".h\"\n"
                        "\n"
                        "uint8_t " +
                        id + "C[" + std::to_string(data1.size()) + "] = {\n" + MakeCSourceArray(data1) + "};\n"
                        "\n" +
                        "uint8_t " +
                        id + "A[" + std::to_string(data2.size()) + "] = {\n" + MakeCSourceArray(data2) + "};\n";

    const std::string nameWoExtension = RemoveExtension(inputFileName);
    FsTools::SaveFile(nameWoExtension + ".c", cFile.data(), cFile.size());
    FsTools::SaveFile(nameWoExtension + ".h", hFile.data(), hFile.size());
    return true;
}

bool convert1(const char* inputFileName) {
    Png png;
    if (!png.load(inputFileName)) return false;

    std::vector<uint8_t> data;
    for (uint32_t y = 0; y + 6 <= png.getHeight(); y += 6) {
        for (uint32_t x = 0; x + 6 <= png.getWidth(); x += 6) {
            for (uint32_t iy = 0; iy < 6; iy += 2) {
                for (uint32_t ix = 0; ix < 6; ix += 2) {
                    uint8_t code, attrib;
                    Encode4(png, x + ix, y + iy, code, attrib);
                    data.push_back(code);
                    data.push_back(attrib);
                }
            }
        }
    }

    std::string id = IdFromFileName(inputFileName);

    std::string hFile =
        "#include <stdint.h>\n"
        "\n"
        "static const unsigned " + id + "Width = 3;\n" +
        "static const unsigned " + id + "Height = 3;\n" +
        "static const unsigned " + id + "Size = " + id + "Width + (" + id + "Height << 8);\n" +
        "\n"
        "extern uint8_t " +
        id + "[" + std::to_string(data.size()) + "];\n";

    std::string cFile = "#include \"" + RemoveExtension(basename(inputFileName)) +
                        ".h\"\n"
                        "\n"
                        "uint8_t " +
                        id + "[" + std::to_string(data.size()) + "] = {\n" + MakeCSourceArray(data) + "};\n";

    const std::string nameWoExtension = RemoveExtension(inputFileName);
    FsTools::SaveFile(nameWoExtension + ".c", cFile.data(), cFile.size());
    FsTools::SaveFile(nameWoExtension + ".h", hFile.data(), hFile.size());
    return true;
}

bool convert2(const char* inputFileName) {
    Png png;
    if (!png.load(inputFileName)) return false;

    std::vector<uint8_t> data;
    for (uint32_t y = 0; y + 2 <= png.getHeight(); y += 2) {
        for (uint32_t x = 0; x + 2 <= png.getWidth(); x += 2) {
            uint8_t code, attrib;
            Encode4(png, x, y, code, attrib);
            data.push_back(code);
            data.push_back(attrib);
        }
    }

    std::string id = IdFromFileName(inputFileName);

    std::string hFile =
        "#include <stdint.h>\n"
        "\n"
        "static const unsigned " + id + "Width = " + std::to_string(png.getWidth() / 2) + ";\n" +
        "static const unsigned " + id + "Height = " + std::to_string(png.getHeight() / 2) + ";\n" +
        "static const unsigned " + id + "Size = " + id + "Width + (" + id + "Height << 8);\n" +
        "\n"
        "extern uint8_t " +
        id + "[" + std::to_string(data.size()) + "];\n";

    std::string cFile = "#include \"" + RemoveExtension(basename(inputFileName)) +
                        ".h\"\n"
                        "\n"
                        "uint8_t " +
                        id + "[" + std::to_string(data.size()) + "] = {\n" + MakeCSourceArray(data) + "};\n";

    const std::string nameWoExtension = RemoveExtension(inputFileName);
    FsTools::SaveFile(nameWoExtension + ".c", cFile.data(), cFile.size());
    FsTools::SaveFile(nameWoExtension + ".h", hFile.data(), hFile.size());
    return true;
}

static uint32_t StrToUint32(const char text[], int base = 0) {
    char* end = nullptr;
    const unsigned long value = strtoul(text, &end, base);
    if (end[0] != '\0' || value >= UINT32_MAX)
        throw std::runtime_error(std::string("Can't convert \"") + text + "\" to number");
    return value;
}

int main(int argc, char** argv) {
    try {
        if (argc != 3) {
            std::cerr << "Micro 80 graph convertor (c) 2025 Alemorf" << std::endl
                      << "Syntax: " << argv[0] << " algorithm_number input_file.png"
                      << std::endl;
            return 1;
        }
        uint32_t algorithm = StrToUint32(argv[1]);
        std::cout << "Convert " << argv[2] << std::endl;
        switch (algorithm) {
            case 0:
                return convert(argv[2]) ? 0 : 1;
            case 1:
                return convert1(argv[2]) ? 0 : 1;
            case 2:
                return convert2(argv[2]) ? 0 : 1;
            default:
                throw std::runtime_error("Unsupported algorithm");
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
