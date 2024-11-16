#pragma once
#include "texture.h"

#include <memory>

// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType {RECTANGLE, ELLIPSE};

class Shape {
public:
    // Фигура после создания имеет нулевые координаты и размер,
    // а также не имеет текстуры
    explicit Shape(ShapeType type) : shape_type_(type) {
        // Заглушка. Реализуйте конструктор самостоятельно
        //(void) type;
    }

    void SetPosition(Point pos) {
        //(void) pos;
        // Заглушка. Реализуйте метод самостоятельно
        top_left_angle_point_ = pos;
    }

    void SetSize(Size size) {
        //(void) size;
        // Заглушка. Реализуйте метод самостоятельно
        shape_size_ = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        //(void) texture;
        // Заглушка. Реализуйте метод самостоятельно
        texture_ = std::move(texture);
    }

    // Рисует фигуру на указанном изображении
	// В зависимости от типа фигуры должен рисоваться либо эллипс, либо прямоугольник
    // Пиксели фигуры, выходящие за пределы текстуры, а также в случае, когда текстура не задана,
    // должны отображаться с помощью символа точка '.'
    // Части фигуры, выходящие за границы объекта image, должны отбрасываться.
    void Draw(Image& image) const {
        //(void) image;
        // Заглушка. Реализуйте метод самостоятельно
        Size image_size = GetImageSize(image);
        Size texture_size = {0, 0};
        if (texture_ != nullptr) {
            texture_size = texture_->GetSize();
        }
        int x_draw_range = std::min(top_left_angle_point_.x + shape_size_.width - 1, image_size.width - 1);
        int y_draw_range = std::min(top_left_angle_point_.y + shape_size_.height - 1, image_size.height - 1);
                
        for (int y = top_left_angle_point_.y; y <= y_draw_range; ++y) {
            for (int x = top_left_angle_point_.x; x <= x_draw_range; ++x) {
                int texture_x = x - top_left_angle_point_.x;
                int texture_y = y - top_left_angle_point_.y;
                if (shape_type_ == ShapeType::RECTANGLE && IsPointInRectangle({texture_x, texture_y}, shape_size_)) {
                    if (texture_x < texture_size.width && texture_y < texture_size.height) {
                        image[y][x] = texture_->GetPixelColor({texture_x, texture_y});
                    }
                    else image[y][x] = '.';
                }
                if (shape_type_ == ShapeType::ELLIPSE && IsPointInEllipse({texture_x, texture_y}, shape_size_)) {
                    if (texture_x < texture_size.width && texture_y < texture_size.height) {
                        image[y][x] = texture_->GetPixelColor({ texture_x, texture_y });
                    }
                    else image[y][x] = '.';
                }
            }
        }
    }
private:
    ShapeType shape_type_;
    Point top_left_angle_point_;
    Size shape_size_;
    std::shared_ptr<Texture> texture_;
};