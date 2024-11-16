#pragma once
#include <array>
#include <cassert>

#include "geom.h"

class Tile {
public:
    // Размер тайла 8*8 пикселей.
    constexpr static int SIZE = 8;

    // Конструктор по умолчанию. Заполняет тайл указанным цветом.
    Tile(char color = ' ') noexcept : pixels_{} {
        /* Реализуйте недостающий код самостоятельно. */
        std::fill(pixels_.begin(), pixels_.end(), color);
        // -------------- не удалять ------------
        assert(instance_count_ >= 0);
        ++instance_count_;  // Увеличиваем счётчик тайлов (для целей тестирования).
        // -------------- не удалять ------------
    }

    Tile(const Tile& other) {
        /* Реализуйте недостающий код самостоятельно. */
        pixels_ = other.pixels_;
        // -------------- не удалять ------------
        assert(instance_count_ >= 0);
        ++instance_count_;  // Увеличиваем счётчик тайлов (для целей тестирования).
        // -------------- не удалять ------------
    }

    ~Tile() {
        // -------------- Не удалять ------------
        --instance_count_;  // Уменьшаем счётчик тайлов.
        assert(instance_count_ >= 0);
        // -------------- не удалять ------------
    }

    /**
     * Изменяет цвет пикселя тайла.
     * Если координаты выходят за пределы тайла, метод ничего не делает.
     */
    void SetPixel(Point p, char color) noexcept {
        /* Реализуйте недостающий код самостоятельно. */
        int pixel_position = p.y * 8 + p.x;
        if (p.x < 0 || p.x >= SIZE || p.y < 0 || p.y >= SIZE) {
            return;
        }
        pixels_[pixel_position] = color;
    }

    /**
     * Возвращает цвет пикселя. Если координаты выходят за пределы тайла, возвращается пробел.
     */
    char GetPixel(Point p) const noexcept {
        /* Реализуйте недостающий функционал самостоятельно. */
        int pixel_position = p.y * 8 + p.x;
        if (p.x < 0 || p.x >= SIZE || p.y < 0 || p.y >= SIZE) {
            return ' ';
        }
        return pixels_[pixel_position];
    }

    // Возвращает количество экземпляра класса Tile в программе.
    static int GetInstanceCount() noexcept {
        // -------------- не удалять ------------
        return instance_count_;
        // -------------- не удалять ------------
    }

private:
    // -------------- не удалять ------------
    inline static int instance_count_ = 0;
    // -------------- не удалять ------------

    /* Разместите здесь поля для хранения пикселей тайла. */
    std::array<char, SIZE * SIZE> pixels_;

};