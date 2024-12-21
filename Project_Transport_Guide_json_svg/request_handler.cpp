/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

#include <set>
#include <stdexcept>
#include <string>
#include <sstream>

#include "domain.h"
#include "request_handler.h"
#include "transport_catalogue.h"

using namespace std::literals;

const std::set<std::string>& RequestHandler::GetBusesAtStop(const std::string_view& stop_name) const {
    return catalogue_.GetStopNameToStop().at(stop_name)->buses_at_stop_;
}

const transport_catalogue::RouteInfo RequestHandler::RouteInformation(const std::string_view& bus_number) const {
    transport_catalogue::RouteInfo route_info = {};
    const transport_catalogue::Bus* bus = catalogue_.FindBus(bus_number);

    if (bus == nullptr) {
        throw std::invalid_argument("Bus is not found"s);
    }
    if (bus->is_roundtrip) {
        route_info.all_stops_counter_ = bus->bus_stops_.size();
    } else {
        route_info.all_stops_counter_ = bus->bus_stops_.size() * 2 - 1;
    }
    
    double geo_route_length = 0.0;
    double route_length = 0.0;

    for (size_t it = 0; it < bus->bus_stops_.size() - 1; ++it) {
        const transport_catalogue::Stop* point_a = bus->bus_stops_[it];
        const transport_catalogue::Stop* point_b = bus->bus_stops_[it + 1];
        if (it == bus->bus_stops_.size() - 1) {
            point_b = bus->bus_stops_[0];
        }
        if (bus->is_roundtrip) {
            route_length += catalogue_.GetDistanceBetweenStops(point_a, point_b);
            geo_route_length += ComputeDistance(point_a->coordinates_, point_b->coordinates_);
        } else {
            route_length += catalogue_.GetDistanceBetweenStops(point_a, point_b) + catalogue_.GetDistanceBetweenStops(point_b, point_a);
            geo_route_length += ComputeDistance(point_a->coordinates_, point_b->coordinates_) * 2;
        }
    }

    route_info.unique_stops_counter_ = catalogue_.UniqueStopsCount(bus_number);
    route_info.route_length_ = route_length;
    route_info.curvature_ = route_length / geo_route_length;

    return route_info;
}

void RequestHandler::ProcessStatRequest(const json::Node& request, std::ostream& output_stream) {
    const int id = request.AsMap().at("id"s).AsInt();
    const std::string& type = request.AsMap().at("type"s).AsString();
    json::Dict response;
    response["request_id"s] = id;

    if (type == "Map"s) {
        std::ostringstream map_in_svg_format;
        RenderMap().Render(map_in_svg_format);
        response["map"s] = map_in_svg_format.str();
    } else {
        const std::string& name = request.AsMap().at("name"s).AsString();
        if (type == "Stop"s) {
            const transport_catalogue::Stop* stop = catalogue_.FindStop(name);
            if (stop != nullptr) {
                const std::set<std::string>& buses = GetBusesAtStop(name);
                response["buses"s] = json::Array(buses.begin(), buses.end());
            } else {
                response["error_message"s] = "not found"s;
            }
        } else if (type == "Bus"s) {
            const transport_catalogue::Bus* bus = catalogue_.FindBus(name);
            if (bus != nullptr) {
                const transport_catalogue::RouteInfo& route_info = RouteInformation(name);
                response["curvature"s] = route_info.curvature_;
                response["route_length"s] = route_info.route_length_;
                response["stop_count"s] = static_cast<int>(route_info.all_stops_counter_);
                response["unique_stop_count"s] = static_cast<int>(route_info.unique_stops_counter_);
            } else {
                response["error_message"s] = "not found"s;
            }
        } else {
            // Обработка ошибки invalid request type
            response["error_message"s] = "Invalid request type"s;
        }
    }
    json::PrintContext context{output_stream, 4, 4};

    json::PrintValue(response, context);
}

void RequestHandler::PrintStatRequest() {
    const json::Array& stat_requests = input_doc_.GetInputDoc().GetRoot().AsMap().at("stat_requests"s).AsArray();
    std::cout << "["s << std::endl;
    bool is_first = true;
    for (const json::Node& request : stat_requests) {
        if (!is_first) {
            std::cout << ","s << std::endl;
        }
        is_first = false;
        ProcessStatRequest(request, std::cout);
    }
    std::cout << std::endl;
    std::cout << "]"s << std::endl;
}

svg::Document RequestHandler::RenderMap() const {
    svg::Document svg;
    svg::ObjectContainer& container = svg;
    size_t color_index = 0;

    for (const auto& bus : catalogue_.GetSortedAllBuses()) {
        if (!bus.second->bus_stops_.empty()) {
            renderer_.RenderBusLine(bus.first, container, color_index);
        }
    }

    color_index = 0;
    for (const auto& bus : catalogue_.GetSortedAllBuses()) {
        if (!bus.second->bus_stops_.empty()) {
            renderer_.RenderBusLabel(bus.first
                                   , renderer_.sphere_projector_(bus.second->bus_stops_[0]->coordinates_)
                                   , container
                                   , color_index);
                                   
            const auto& last_stop = bus.second->bus_stops_.size() - 1;                       
            if (!bus.second->is_roundtrip && bus.second->bus_stops_[0] != bus.second->bus_stops_[last_stop]) {
                renderer_.RenderBusLabel(bus.first
                                   , renderer_.sphere_projector_(bus.second->bus_stops_[last_stop]->coordinates_)
                                   , container
                                   , color_index);
            }
        }
        ++color_index;
    }

    for (const auto& stop : catalogue_.GetSortedAllStops()) {
        if (!stop.second->buses_at_stop_.empty()) {
            renderer_.RenderStop(stop.first, container);
        }
    }

    for (const auto& stop : catalogue_.GetSortedAllStops()) {
        if (!stop.second->buses_at_stop_.empty()) {
            renderer_.RenderStopLabel(stop.first, renderer_.sphere_projector_(stop.second->coordinates_), container);
        }
    }

    return svg;
}