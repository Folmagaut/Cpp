#include "ppm_image.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string_view>

using namespace std;

int Sum(img_lib::Color c) {
    return to_integer<int>(c.r) + to_integer<int>(c.g) + to_integer<int>(c.b);
}

// реализуйте оператор Собеля
img_lib::Image Sobel(const img_lib::Image& image) {
    int w = image.GetWidth();
    int h = image.GetHeight();
    img_lib::Image sobel_image(w, h, img_lib::Color::Black());
    
    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            int tl = Sum(image.GetPixel(x - 1, y - 1));
            int tc = Sum(image.GetPixel(x, y - 1));
            int tr = Sum(image.GetPixel(x + 1, y - 1));
            int bl = Sum(image.GetPixel(x - 1, y + 1));
            int bc = Sum(image.GetPixel(x, y + 1));
            int br = Sum(image.GetPixel(x + 1, y + 1));
            int cl = Sum(image.GetPixel(x - 1, y));
            int cr = Sum(image.GetPixel(x + 1, y));

            int gx = -tl - 2 * tc - tr + bl + 2 * bc + br;
            int gy = -tl - 2 * cl - bl + tr + 2 * cr + br;

            double sobel_operator = sqrt(gx * gx + gy * gy);
            double mix_pix = std::clamp(sobel_operator, 0., 255.);
            //byte mix_pix = static_cast<byte>(clamp<double>(sqrt(gx * gx + gy * gy), 0., 255.));
            img_lib::Color* color = &sobel_image.GetPixel(x, y);

            color->r = static_cast<byte>(mix_pix);
            color->g = static_cast<byte>(mix_pix);
            color->b = static_cast<byte>(mix_pix);
        }
    }

    return sobel_image;
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <input image> <output image>"sv << endl;
        return 1;
    }

    auto image = img_lib::LoadPPM(argv[1]);
    if (!image) {
        cerr << "Error loading image"sv << endl;
        return 2;
    }

    image = Sobel(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}