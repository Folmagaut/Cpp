#include "ppm_image.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

static const string_view PPM_SIG = "P6"sv;
static const int PPM_MAX = 255;

// реализуйте эту функцию самостоятельно
bool SavePPM(const Path& file, const Image& image) {
    ofstream ofs(file, ios::binary);
    ofs << PPM_SIG << "\n";
    ofs << image.GetWidth() << ' ' << image.GetHeight() << "\n";
    ofs << PPM_MAX << "\n";

    const int w = image.GetWidth();
    const int h = image.GetHeight();
    
    vector<char> temp_pixels(w * 3); // временный вектор пикселей в три ширины изображения

    for (int y = 0; y < h; ++y) {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < w; ++x) {
            temp_pixels[x * 3 + 0] = static_cast<char>(line[x].r);
            temp_pixels[x * 3 + 1] = static_cast<char>(line[x].g);
            temp_pixels[x * 3 + 2] = static_cast<char>(line[x].b);
        }
        ofs.write(temp_pixels.data(), w * 3);
    }
    return static_cast<bool>(ofs);
}

Image LoadPPM(const Path& file) {
    // открываем поток с флагом ios::binary
    // поскольку будем читать даные в двоичном формате
    ifstream ifs(file, ios::binary);
    std::string sign;
    int w, h, color_max;

    // читаем заголовок: он содержит формат, размеры изображения
    // и максимальное значение цвета
    ifs >> sign >> w >> h >> color_max;

    // мы поддерживаем изображения только формата P6
    // с максимальным значением цвета 255
    if (sign != PPM_SIG || color_max != PPM_MAX) {
        return {};
    }

    // пропускаем один байт - это конец строки
    const char next = ifs.get();
    if (next != '\n') {
        return {};
    }

    Image result(w, h, Color::Black());
    std::vector<char> temp_pixels(w * 3);

    for (int y = 0; y < h; ++y) {
        Color* line = result.GetLine(y);
        ifs.read(temp_pixels.data(), w * 3);

        for (int x = 0; x < w; ++x) {
            line[x].r = static_cast<byte>(temp_pixels[x * 3 + 0]);
            line[x].g = static_cast<byte>(temp_pixels[x * 3 + 1]);
            line[x].b = static_cast<byte>(temp_pixels[x * 3 + 2]);
        }
    }

    return result;
}

}  // namespace img_lib