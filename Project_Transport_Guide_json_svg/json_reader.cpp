/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

#include "geo.h"
#include "json_reader.h"
#include "request_handler.h"

using namespace std::literals;
void JsonReader::FillCatalogueWithRequests(transport_catalogue::TransportCatalogue& catalogue) {
    const json::Array& base_requests = input_doc_.GetRoot().AsMap().at("base_requests"s).AsArray();

    std::vector<transport_catalogue::Distance> distances = {};

    for (const json::Node& request : base_requests) {
        std::string request_type = "Stop"s;
        ProcessBaseRequest(request, request_type, catalogue, distances);
    }

    for (const json::Node& request : base_requests) {
        std::string request_type = "Bus"s;
        ProcessBaseRequest(request, request_type, catalogue, distances);
    }

    for (const auto& distance : distances) {
        const transport_catalogue::Stop* pnt_a = catalogue.FindStop(distance.point_a_);
        const transport_catalogue::Stop* pnt_b = catalogue.FindStop(distance.point_b_);
        catalogue.SetDistanceBetweenStops(pnt_a, pnt_b, distance.distance_);
    }
}

void JsonReader::ProcessBaseRequest(const json::Node& request, const std::string& request_type, transport_catalogue::TransportCatalogue& catalogue,
                                    std::vector<transport_catalogue::Distance>& distances) {
    const std::string& type = request.AsMap().at("type"s).AsString();

    if (type == "Stop" && type == request_type) {
        const std::string& stop_name = request.AsMap().at("name"s).AsString();
        const double latitude = request.AsMap().at("latitude"s).AsDouble();
        const double longitude = request.AsMap().at("longitude"s).AsDouble();
        geo::Coordinates coordinates = {latitude, longitude};
        catalogue.AddStop(stop_name, coordinates);

        const json::Dict& road_distances = request.AsMap().at("road_distances"s).AsMap();
    for (const auto& [other_stop_name, distance] : road_distances) {
        distances.push_back({stop_name, other_stop_name, distance.AsInt()});
    }
        
    } else if (type == "Bus" && type == request_type) {
        const std::string& bus_name = request.AsMap().at("name"s).AsString();
        const json::Array& stops = request.AsMap().at("stops"s).AsArray();
        bool is_roundtrip = request.AsMap().at("is_roundtrip"s).AsBool();

        std::vector<const transport_catalogue::Stop*> bus_stops;
        for (const json::Node& stop_name_node : stops) {
            const std::string& stop_name = stop_name_node.AsString();
            const transport_catalogue::Stop* stop = catalogue.FindStop(stop_name);
            if (!stop) {
                // Обработка ошибки: stop not found
            }
            bus_stops.push_back(stop);
        }
        catalogue.AddBus(bus_name, bus_stops, is_roundtrip);
    } else {
        // Обработка ошибки: invalid request type
    }
}

const json::Document& JsonReader::GetInputDoc() const {
    return input_doc_;
}