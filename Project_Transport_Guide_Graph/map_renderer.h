/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

// без изменений с прошлого спринта
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "svg.h"

inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector {
public:
    SphereProjector() = default;
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

struct RenderSettings {
    double width = 0.0; // от 0 до 100000
    double height = 0.0; // от 0 до 100000
    double padding = 0.0; // не меньше 0 и не меньше min(width, height)/2
    double line_width = 0.0; // 0 ... 100000
    double stop_radius = 0.0; // 0 ... 100000
    int bus_label_font_size = 0; // 0 ... 100000
    svg::Point bus_label_offset = {0.0, 0.0}; // Массив двух double. Задаёт значения dx и dy SVG-элемента <text>. от –100000 до 100000
    int stop_label_font_size = 0; // 0...100000
    svg::Point stop_label_offset = {0.0, 0.0}; // Массив двух double. Задаёт значения dx и dy SVG-элемента <text>. от –100000 до 100000
    svg::Color underlayer_color = { svg::NoneColor }; // цвет подложки
    double underlayer_width = 0.0; // Задаёт значение атрибута stroke-width элемента <text>. Вещественное число в диапазоне от 0 до 100000.
    std::vector<svg::Color> color_palette{};
};

class MapRenderer {
public:
    explicit MapRenderer(const transport_catalogue::TransportCatalogue& catalogue, const JsonReader& doc);

    // Перенёс. На самом деле с этим методом та же история - его в прекоде было предложено вынести
    // в RequestHandler. Непонятно, зачем вводить людей в заблуждение.
    svg::Document RenderMap() const;

private:
    const transport_catalogue::TransportCatalogue& catalogue_;
    const JsonReader& doc_;
    SphereProjector sphere_projector_;
    // Настройки рендера
    RenderSettings render_settings_;
    svg::Color ParseColor(const json::Node& color_node) const;
    void RenderBusLine(const std::string_view bus, svg::ObjectContainer& container, size_t& color_index) const;
    void RenderBusLabel(const std::string_view bus, const svg::Point& point, svg::ObjectContainer& container, size_t& color_index) const;
    void RenderStop(const std::string_view stop, svg::ObjectContainer& container) const;
    void RenderStopLabel(const std::string_view stop, const svg::Point& point, svg::ObjectContainer& container) const;
};