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

using namespace std::literals;
void JsonReader::FillCatalogueWithRequests(/* std::istream& input_stream,  */transport_catalogue::TransportCatalogue& catalogue) {
    //json::Document doc = json::Load(input_stream);
    const json::Array& base_requests = input_doc_.GetRoot().AsMap().at("base_requests"s).AsArray();
    const json::Array& stat_requests = input_doc_.GetRoot().AsMap().at("stat_requests"s).AsArray();
    std::vector<transport_catalogue::Distance> distances = {};

    for (const json::Node& request : base_requests) {
        std::string stop = "Stop"s;
        ProcessBaseRequest(request, stop, catalogue, distances);
    }

    for (const json::Node& request : base_requests) {
        std::string stop = "Bus"s;
        ProcessBaseRequest(request, stop, catalogue, distances);
    }

    for (const auto& distance : distances) {
        const transport_catalogue::Stop* pnt_a = catalogue.FindStop(distance.point_a_);
        const transport_catalogue::Stop* pnt_b = catalogue.FindStop(distance.point_b_);
        catalogue.SetDistanceBetweenStops(pnt_a, pnt_b, distance.distance_);
    }

    std::cout << "["s << std::endl;
    bool is_first = true;
    for (const json::Node& request : stat_requests) {
        if (!is_first) {
            std::cout << ","s << std::endl;
        }
        is_first = false;
        ProcessStatRequest(request, catalogue, std::cout);
    }
    std::cout << std::endl;
    std::cout << "]"s << std::endl;
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
                // Handle error: stop not found
            }
            bus_stops.push_back(stop);
        }
        catalogue.AddBus(bus_name, bus_stops, is_roundtrip);
    } else {
        // Handle error: invalid request type
    }
}

void JsonReader::ProcessStatRequest(const json::Node& request, transport_catalogue::TransportCatalogue& catalogue, std::ostream& output_stream) {
    const int id = request.AsMap().at("id"s).AsInt();
    const std::string& type = request.AsMap().at("type"s).AsString();
    const std::string& name = request.AsMap().at("name"s).AsString();

    json::Dict response;
    response["request_id"s] = id;

    if (type == "Stop"s) {
        const transport_catalogue::Stop* stop = catalogue.FindStop(name);
        if (stop) {
            const std::set<std::string>& buses = catalogue.GetBusesAtStop(name);
            response["buses"s] = json::Array(buses.begin(), buses.end());
        } else {
            response["error_message"s] = "not found"s;
        }
    } else if (type == "Bus"s) {
        const transport_catalogue::Bus* bus = catalogue.FindBus(name);
        if (bus) {
            const transport_catalogue::RouteInfo& route_info = catalogue.RouteInformation(name);
            response["curvature"s] = route_info.curvature_;
            response["route_length"s] = route_info.route_length_;
            response["stop_count"s] = static_cast<int>(route_info.all_stops_counter_);
            response["unique_stop_count"s] = static_cast<int>(route_info.unique_stops_counter_);
        } else {
            response["error_message"s] = "not found"s;
        }
    } else {
        // Handle invalid request type
        response["error_message"s] = "invalid request type"s;
    }
    json::PrintContext context{output_stream};

    json::PrintValue(response, context);
}