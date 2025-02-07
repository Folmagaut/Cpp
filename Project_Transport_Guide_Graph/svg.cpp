// без изменений с прошлого спринта

#include "svg.h"

namespace svg {
using namespace std;
using namespace literals;
using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

//---------------- ostream operator<<----------------
std::ostream& operator<<(std::ostream& out, Color& color) {
    //ostringstream strm;
    std::visit(ColorPrinter{out}, color);
    return out;
}

std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap) {
    switch (stroke_line_cap) {
        case StrokeLineCap::BUTT :
            return out << "butt"sv;
            break;
        case StrokeLineCap::ROUND :
            return out << "round"sv;
            break;
        case StrokeLineCap::SQUARE :
            return out << "square"sv;
            break;
        default :
            return out;
    }
}

std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join) {
    switch (stroke_line_join) {
        case StrokeLineJoin::ARCS :
            return out << "arcs"sv;
            break;
        case StrokeLineJoin::BEVEL :
            return out << "bevel"sv;
            break;
        case StrokeLineJoin::MITER :
            return out << "miter"sv;
            break;
        case StrokeLineJoin::MITER_CLIP :
            return out << "miter-clip"sv;
            break;
        case StrokeLineJoin::ROUND :
            return out << "round"sv;
            break;
        default :
            return out;
    }
}

namespace {

template <typename T>
inline void RenderValue(std::ostream& out, const T& value) {
    out << value;
}

void HtmlEncodeString(std::ostream& out, std::string_view sv) {
    for (char c : sv) {
        switch (c) {
            case '"':
                out << "&quot;"sv;
                break;
            case '<':
                out << "&lt;"sv;
                break;
            case '>':
                out << "&gt;"sv;
                break;
            case '&':
                out << "&amp;"sv;
                break;
            case '\'':
                out << "&apos;"sv;
                break;
            default:
                out.put(c);
        }
    }
}

template <>
void RenderValue<std::string>(std::ostream& out, const std::string& s) {
    HtmlEncodeString(out, s);
}

template <typename AttrType>
void RenderAttr(std::ostream& out, std::string_view name, const AttrType& value) {
    using namespace std::literals;
    out << name << "=\""sv;
    RenderValue(out, value);
    out.put('"');
}

}  // namespace

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
    std::ostream& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y;
    out << "\" "sv << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(context.out); // атрибуты PathProps
    out << "/>"sv;

}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    std::ostream& out = context.out;
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
    out << "\""sv;
    RenderAttrs(context.out); // атрибуты PathProps
    out << "/>"sv;
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
    font_size_ = size;
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
    std::ostream& out = context.out;
    out << "<text"sv;
    RenderAttrs(context.out); // атрибуты PathProps
    RenderAttr(out, " x"sv, pos_.x);
    RenderAttr(out, " y"sv, pos_.y);
    RenderAttr(out, " dx"sv, offset_.x);
    RenderAttr(out, " dy"sv, offset_.y);
    RenderAttr(out, " font-size"sv, font_size_);
    if (!font_family_.empty()) {
        RenderAttr(out, " font-family"sv, font_family_);
    }
    if (!font_weight_.empty()) {
        RenderAttr(out, " font-weight"sv, font_weight_);
    }
    out.put('>');
    HtmlEncodeString(out, data_);
    out << "</text>"sv;
}

// ---------- Document ------------------

void Document::AddPtr(unique_ptr<Object>&& obj) {
    objects_.emplace_back(move(obj));
}
void Document::Render(ostream& out) const {
    RenderContext ctx(out, 2, 2);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << endl;
    for (const auto& obj : objects_) {
        obj->Render(ctx);
    }
    out << "</svg>"sv;
}

}  // namespace svg