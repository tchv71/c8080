/*
 * Convert PNG files for 8-bit computer Specialist
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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
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

static void ConvertImage(const std::string &inputFileName, const std::string &id, uint32_t cw, uint32_t ch,
                         bool modeColors, bool modePack) {
    Png png;
    if (!png.load(inputFileName.c_str()))
        throw std::runtime_error("Can't open file " + inputFileName);

    if (cw % 8)
        throw std::runtime_error("The width must be a multiple of 8, cw = " + std::to_string(cw));

    if (png.getWidth() % 8)
        throw std::runtime_error("The width must be a multiple of 8, png.width = " + std::to_string(png.getWidth()));

    uint32_t imageSize;
    if (cw != 0 && ch != 0) {
        imageSize = (cw / 8) * ch;
        if (modeColors && !modePack)
            imageSize *= 2;
    } else {
        imageSize = 0;
        cw = png.getWidth();
        ch = png.getHeight();
    }

    std::vector<uint8_t> data, data2;
    for (int ix = 0; ix < png.getWidth(); ix += cw) {
        for (int iy = 0; iy < png.getHeight(); iy += ch) {
            int pcc = 0;
            for (int x = 0; x < cw; x += 8) {
                for (int y = 0; y < ch; y++) {
                    int d = 0, cc1 = 0xF0;
                    uint32_t clr1;
                    for (int sx = 0; sx < 8; sx++) {
                        uint32_t clr = png.getPixel(ix + x + sx, iy + y);
                        int c1;
                        clr = (clr & 0x00FF00) | ((clr & 0x0000FF) << 16) | ((clr & 0xFF0000) >> 16);
                        switch (clr) {
                            case 0xFFFFFF:
                                c1 = 0x00;
                                break;
                            case 0xFFFF00:
                                c1 = 0x10;
                                break;
                            case 0xFF00FF:
                                c1 = 0x40;
                                break;
                            case 0xFF0000:
                                c1 = 0x50;
                                break;
                            case 0x00FFFF:
                                c1 = 0x80;
                                break;
                            case 0x00FF00:
                                c1 = 0x90;
                                break;
                            case 0x0000FF:
                                c1 = 0xC0;
                                break;
                            default:
                                c1 = 0xF0;
                        }
                        d <<= 1;
                        if (c1 != 0xF0) {
                            if (cc1 == 0xF0 || cc1 == 0)
                                clr1 = clr, cc1 = c1;
                            d |= 1;
                        }
                    }
                    if (cc1 != 0xF0)
                        pcc = cc1;
                    if (modeColors) {
                        if (modePack)
                            data2.push_back(pcc);
                        else
                            data.push_back(pcc);
                    }
                    data.push_back(d);
                }
            }
        }
    }

    if (modePack) {
        PackMegalz(data);
        PackMegalz(data2);
    }

    hFile +=
        "\n"
        "static const unsigned " +
        id + "Width = " + std::to_string(cw) + ";\n" + "static const unsigned " + id +
        "Height = " + std::to_string(ch) + ";\n" + "static const unsigned " + id + "Size = " + id + "Width / 8 + (" +
        id + "Height << 8);\n";

    if (imageSize == 0) {
        hFile += "extern uint8_t " + id + "[" + std::to_string(data.size()) + "];\n";

        cFile +=
            "\n"
            "uint8_t " +
            id + "[" + std::to_string(data.size()) + "] = {\n" + MakeCSourceArray(data.data(), data.size()) + "};\n";

        if (!data2.empty()) {
            hFile += "extern uint8_t " + id + "Colors[" + std::to_string(data2.size()) + "];\n";

            cFile +=
                "\n"
                "uint8_t " +
                id + "Colors[" + std::to_string(data2.size()) + "] = {\n" +
                MakeCSourceArray(data2.data(), data2.size()) + "};\n";
        }
    } else {
        uint32_t imageCount = data.size() / imageSize;

        hFile += "extern uint8_t " + id + "[" + std::to_string(imageCount) + "][" + std::to_string(imageSize) + "];\n";

        cFile +=
            "\n"
            "uint8_t " +
            id + "[" + std::to_string(imageCount) + "][" + std::to_string(imageSize) + "] = {\n" +
            MakeCSourceArray2(data.data(), data.size(), imageCount, imageSize) + "};\n";

        if (!data2.empty()) {
            hFile += "extern uint8_t " + id + "Colors[" + std::to_string(imageCount) + "][" +
                     std::to_string(imageSize) + "];\n";

            cFile +=
                "\n"
                "uint8_t " +
                id + "Colors[" + std::to_string(imageCount) + "][" + std::to_string(imageSize) + "] = {\n" +
                MakeCSourceArray2(data2.data(), data2.size(), imageCount, imageSize) + "};\n";
        }
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

int main(int argc, char **argv) {
    try {
        std::cout << "Specialist graph convertor (c) 2025 Alemorf" << std::endl;

        if (argc != 2) {
            std::cerr << "Using: " << argv[0] << " path" << std::endl;
            return 1;
        }

        hFile =
            "// Autogenerated\n"
            "#include <stdint.h>\n";

        cFile =
            "// Autogenerated\n"
            "#include \"resources.h\"\n";

        for (const auto &entry : std::filesystem::directory_iterator(argv[1])) {
            if (entry.path().extension() == ".png") {
                std::cout << "Process file " << entry.path().string() << std::endl;
                std::string fileName = entry.path().stem();
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
