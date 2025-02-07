#include <algorithm>

#include "map_renderer.h"

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

MapRenderer::MapRenderer(const transport_catalogue::TransportCatalogue& catalogue, const JsonReader& doc)
    : catalogue_(catalogue), doc_(doc) {

    const auto& render_settings = doc_.GetInputDoc().GetRoot().AsDict().at("render_settings"s).AsDict();
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
    coords.reserve(catalogue_.GetAllStops().size());
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

void MapRenderer::RenderBusLine(const std::string_view bus, svg::ObjectContainer& container, size_t& color_index) const {
    size_t const size = render_settings_.color_palette.size();
    auto curr_color = render_settings_.color_palette[color_index % size];
    svg::Polyline line;
    const auto& current_bus = catalogue_.FindBus(bus);
    line.SetFillColor(svg::NoneColor);
    line.SetStrokeColor(curr_color); // Раньше здесь была ужасная функция для получения цвета автобуса, а теперь может быть Ваша реклама :)
    line.SetStrokeWidth(render_settings_.line_width);
    line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    
    std::vector<const transport_catalogue::Stop*> current_bus_stops{current_bus->bus_stops_.begin(), current_bus->bus_stops_.end()};
    if (current_bus->is_roundtrip == false) {
        auto pos = current_bus_stops.end(); // если итератор заранее не обозначить, а ниже в insert просто написать current_bus_stops.end(), то я ловлю ошибку
        current_bus_stops.insert(pos, std::next(current_bus->bus_stops_.rbegin()), current_bus->bus_stops_.rend());
    }
    
    for (const auto* stop : current_bus_stops) {
        line.AddPoint(sphere_projector_(stop->coordinates_));
    }
    container.Add(line);
    color_index += 1;
}

void MapRenderer::RenderBusLabel(const std::string_view bus, const svg::Point& point, svg::ObjectContainer& container, size_t& color_index) const {
    size_t const size = render_settings_.color_palette.size();
    auto curr_color = render_settings_.color_palette[color_index % size];
    svg::Text text;
    svg::Text underlayer;
    text.SetPosition(point);
    text.SetOffset(render_settings_.bus_label_offset);
    // Установка цвета, шрифта и других свойств текста
    text.SetFontSize(render_settings_.bus_label_font_size);
    text.SetFontFamily("Verdana"s);
    text.SetFontWeight("bold"s);
    text.SetData(catalogue_.FindBus(bus)->bus_number_);
    text.SetFillColor(curr_color);

    // underlayer
    underlayer.SetPosition(point);
    underlayer.SetOffset(render_settings_.bus_label_offset);
    // Установка цвета, шрифта и других свойств подложкки
    underlayer.SetFontSize(render_settings_.bus_label_font_size);
    underlayer.SetFontFamily("Verdana"s);
    underlayer.SetFontWeight("bold"s);
    underlayer.SetData(catalogue_.FindBus(bus)->bus_number_);
    // дополнительные настройки
    underlayer.SetFillColor(render_settings_.underlayer_color);
    underlayer.SetStrokeColor(render_settings_.underlayer_color);
    underlayer.SetStrokeWidth(render_settings_.underlayer_width);
    underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

    container.Add(underlayer);
    container.Add(text);
}

void MapRenderer::RenderStop(const std::string_view stop, svg::ObjectContainer& container) const {
    svg::Circle circle;
    circle.SetCenter(sphere_projector_(catalogue_.FindStop(stop)->coordinates_));
    circle.SetRadius(render_settings_.stop_radius);
    circle.SetFillColor("white");

    container.Add(circle);
}

void MapRenderer::RenderStopLabel(const std::string_view stop, const svg::Point& point, svg::ObjectContainer& container) const {
    svg::Text text;
    svg::Text underlayer;
    text.SetPosition(point);
    text.SetOffset(svg::Point(render_settings_.stop_label_offset));
    // Установка цвета, шрифта и других свойств текста
    text.SetFontSize(render_settings_.stop_label_font_size);
    text.SetFontFamily("Verdana"s);
    text.SetData(catalogue_.FindStop(stop)->stop_name_);
    text.SetFillColor("black");

    // underlayer
    underlayer.SetPosition(point);
    underlayer.SetOffset(svg::Point(render_settings_.stop_label_offset));
    // Установка цвета, шрифта и других свойств текста
    underlayer.SetFontSize(render_settings_.stop_label_font_size);
    underlayer.SetFontFamily("Verdana"s);
    underlayer.SetData(catalogue_.FindStop(stop)->stop_name_);
    underlayer.SetFillColor(render_settings_.underlayer_color);
    underlayer.SetStrokeColor(render_settings_.underlayer_color);
    underlayer.SetStrokeWidth(render_settings_.underlayer_width);
    underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

    container.Add(underlayer);
    container.Add(text);
}

svg::Document MapRenderer::RenderMap() const {
    svg::Document svg;
    svg::ObjectContainer& container = svg;
    size_t color_index = 0;

    for (const auto& bus : catalogue_.GetSortedAllBuses()) {
        if (!bus.second->bus_stops_.empty()) {
            RenderBusLine(bus.first, container, color_index);
        }
    }

    color_index = 0;
    for (const auto& bus : catalogue_.GetSortedAllBuses()) {
        if (!bus.second->bus_stops_.empty()) {
            RenderBusLabel(bus.first
                                   , sphere_projector_(bus.second->bus_stops_[0]->coordinates_)
                                   , container
                                   , color_index);
                                   
            const auto& last_stop = bus.second->bus_stops_.size() - 1;                       
            if (!bus.second->is_roundtrip && bus.second->bus_stops_[0] != bus.second->bus_stops_[last_stop]) {
                RenderBusLabel(bus.first
                                   , sphere_projector_(bus.second->bus_stops_[last_stop]->coordinates_)
                                   , container
                                   , color_index);
            }
        }
        ++color_index;
    }

    for (const auto& stop : catalogue_.GetSortedAllStops()) {
        if (!stop.second->buses_at_stop_.empty()) {
            RenderStop(stop.first, container);
        }
    }

    for (const auto& stop : catalogue_.GetSortedAllStops()) {
        if (!stop.second->buses_at_stop_.empty()) {
            RenderStopLabel(stop.first, sphere_projector_(stop.second->coordinates_), container);
        }
    }

    return svg;
}