#include "svg.h"

namespace svg {
using namespace std;
using namespace literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv << "r=\""sv << radius_ << "\" "sv << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool is_first = true;
    for (const Point& point : points_) {
        if (is_first) {
            out << point.x << "," << point.y;
            is_first = false;
        } else {
            out << " "sv << point.x << "," << point.y;
        }
    }
    out << "\" />"sv;
    }

// ---------- Text ------------------

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}
Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}
Text& Text::SetFontFamily(string font_family) {
    font_family_ = move(font_family);
    return *this;
}
Text& Text::SetFontWeight(string font_weight) {
    font_weight_ = move(font_weight);
    return *this;
}
Text& Text::SetData(string data) {
    data_ = move(data);
    return *this;
}
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv
    << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv
    << "font-size=\""sv << size_ << "\""sv;
    if (!font_family_.empty()) {
        out << " font-family=\""sv << font_family_ << "\" "sv;
    }
    if (!font_weight_.empty()) {
        out << "font-weight=\""sv << font_weight_ << "\""sv;
    }
    out << ">"sv << data_ << "</text>"sv;
}

// ---------- Document ------------------

/* void Document::Add(Object obj) {
    objects_.emplace_back(make_unique<Object>(move(obj)));
} */

void Document::AddPtr(unique_ptr<Object>&& obj) {
    objects_.emplace_back(move(obj));
}

void Document::Render(ostream& out) const {
    RenderContext ctx(cout, 2, 2);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << endl
    << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << endl;
    for (const auto& obj : objects_) {
        obj->Render(ctx);
    }
    out << "</svg>"sv;
}

}  // namespace svg