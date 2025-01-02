/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
#include <algorithm>

#include "map_renderer.h"

// ... методы и функции
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

MapRenderer::MapRenderer(const transport_catalogue::TransportCatalogue& catalogue, const JsonReader& doc)
    : catalogue_(catalogue), doc_(doc) {

    const auto& render_settings = doc_.GetInputDoc().GetRoot().AsMap().at("render_settings"s).AsMap();
    render_settings_.width = render_settings.at("width"s).AsDouble();
    render_settings_.height = render_settings.at("height"s).AsDouble();
    render_settings_.padding = render_settings.at("padding"s).AsDouble();
    render_settings_.stop_radius = render_settings.at("stop_radius"s).AsDouble();
    render_settings_.line_width = render_settings.at("line_width"s).AsDouble();
    render_settings_.bus_label_font_size = render_settings.at("bus_label_font_size"s).AsInt();
    const json::Array& bus_label_offset = render_settings.at("bus_label_offset"s).AsArray();
    render_settings_.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings_.stop_label_font_size = render_settings.at("stop_label_font_size"s).AsInt();
    const json::Array& stop_label_offset = render_settings.at("stop_label_offset"s).AsArray();
    render_settings_.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };
    render_settings_.underlayer_color = ParseColor(render_settings.at("underlayer_color"s));
    render_settings_.underlayer_width = render_settings.at("underlayer_width"s).AsDouble();
    
    const json::Array& color_palette = render_settings.at("color_palette"s).AsArray();
    for (const auto& color_node : color_palette) {
        render_settings_.color_palette.push_back(ParseColor(color_node));
    }

    std::vector<geo::Coordinates> coords;
    for (const auto& stop : catalogue_.GetAllStops()) {
        if (!stop.buses_at_stop_.empty()) {
            coords.push_back(stop.coordinates_);
        }
    }
    SphereProjector sphere_projector(coords.begin()
                                   , coords.end()
                                   , render_settings_.width
                                   , render_settings_.height
                                   , render_settings_.padding);

    sphere_projector_ = std::move(sphere_projector);
}

svg::Color MapRenderer::ParseColor(const json::Node& color_node) const {
    if (color_node.IsString()) {
        return color_node.AsString();
    } else if (color_node.IsArray()) {
        const auto& array = color_node.AsArray();
        if (array.size() == 3) {
            return svg::Rgb{static_cast<uint8_t>(array[0].AsInt()),
                            static_cast<uint8_t>(array[1].AsInt()),
                            static_cast<uint8_t>(array[2].AsInt())};
        } else if (array.size() == 4) {
            return svg::Rgba{static_cast<uint8_t>(array[0].AsInt()),
                             static_cast<uint8_t>(array[1].AsInt()),
                             static_cast<uint8_t>(array[2].AsInt()),
                             array[3].AsDouble()};
        }
    }
    // Обработка ошибок: неизвестный формат цвета
    throw std::runtime_error("Invalid color format"s);
}



// ... вспомогательные методы
int index = 0;
void MapRenderer::RenderBusLine(const std::string_view bus, svg::ObjectContainer& container) const {
    svg::Polyline line;
    size_t const size = render_settings_.color_palette.size();
    auto curr_color = render_settings_.color_palette[index%size];
    index += 1;
    const auto& current_bus = catalogue_.FindBus(bus);
    line.SetFillColor(svg::NoneColor);
    line.SetStrokeColor(curr_color); // Функция для получения цвета автобуса
    line.SetStrokeWidth(render_settings_.line_width);
    line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    
    std::vector<const transport_catalogue::Stop*> current_bus_stops{current_bus->bus_stops_.begin(), current_bus->bus_stops_.end()};
    if (current_bus->is_roundtrip == false) {
        auto pos = current_bus_stops.end(); // если итератор заранее не обозначить, а ниже в insert просто написать current_bus_stops.end(), то будет ошибка
        current_bus_stops.insert(pos, std::next(current_bus->bus_stops_.rbegin()), current_bus->bus_stops_.rend());
    }
    
    for (const auto* stop : current_bus_stops) {
        line.AddPoint(sphere_projector_(stop->coordinates_));
    }
    container.Add(line);
}

svg::Document MapRenderer::RenderMap() const {
    svg::Document svg;
    svg::ObjectContainer& container = svg;

    for (const auto& bus : catalogue_.GetSortedAllBuses()) {
        if (!bus.second->bus_stops_.empty()) {
            RenderBusLine(bus.first, container);
        }
    }

    return svg;
}