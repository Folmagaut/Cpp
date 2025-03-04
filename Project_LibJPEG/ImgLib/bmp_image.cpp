#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    // поля заголовка Bitmap File Header
    char char_b = 'B'; // 1 байт
    char char_m = 'M'; // 1 байт
    uint32_t header_and_data_size;  // 4 байта, data = отступ * высота
    int32_t reserved = 0; // 4 байта, инициализированы нулями
    uint32_t padding = 54; // 4 байта, сумма байт двух заголовков 14 + 40
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    uint32_t header_size = 40; // 4 байта, размер 2 части заголовка 40
    int32_t width; // 4 байта
    int32_t height; // 4 байта 
    uint16_t plane_num = 1; // 2 байта, количество плоскостей
    uint16_t bit_per_pixel = 24; // 2 байта, 24 бит на пиксель
    uint32_t compression_type = 0; // 4 байта, без сжатия
    uint32_t data_size; // 4 байта, отступ * высота
    int32_t h_resolution = 11811; // 4 байта
    int32_t v_resolution = 11811; // 4 байта
    int32_t colors_num = 0; // 4 байта - количество использованных цветов, 0 - не определено
    int32_t significant_colors = 0x1000000; // 4 байта
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине:
// деление, а затем умножение на 4 округляет до числа, кратного четырём
// прибавление тройки гарантирует, что округление будет вверх
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image) {
    ofstream out(file, ios::binary);
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    const int w = image.GetWidth();
    const int h = image.GetHeight();
    const int padding = GetBMPStride(w);
    std::vector<char> buff(padding);

    file_header.header_and_data_size = file_header.padding + padding * h;
    info_header.width = w;
    info_header.height = h;
    info_header.data_size = padding * h;
    // приведение к указателю на char для записи бинарных данных
    out.write(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
    out.write(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));

    for (int y = 0; y < h; ++y) {
        const Color* line = image.GetLine(h - 1 - y);
        for (int x = 0; x < w; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(buff.data(), padding);
    }

    return out.good();
}

// напишите эту функцию
Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    ifs.read(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
    ifs.read(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));

    if (file_header.char_b != 'B'
        || file_header.char_m != 'M'
        || file_header.padding != 54
        || info_header.header_size != 40
        || info_header.plane_num != 1
        || info_header.bit_per_pixel != 24
        || info_header.compression_type != 0
        || info_header.h_resolution != 11811
        || info_header.v_resolution != 11811
        || info_header.colors_num != 0
        || info_header.significant_colors != 0x1000000) {
        return {};
    }
    const int w = info_header.width;
    const int h = info_header.height;
    const int padding = GetBMPStride(w);
    std::vector<char> buff(padding);
    Image result(w, h, Color::Black());

    for (int y = 0; y < h; ++y) {
        Color* line = result.GetLine(h - 1 - y);
        ifs.read(buff.data(), padding);

        for (int x = 0; x < w; ++x) {
            line[x].b = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<byte>(buff[x * 3 + 2]);
        }
    }

    return result;
}

}  // namespace img_lib