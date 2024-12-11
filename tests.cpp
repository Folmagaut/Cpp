#pragma once

#include "transport_catalogue.h"
#include "json.h"

namespace transport_catalogue {

class JsonReader {
public:
    void Read(std::istream& input_stream, TransportCatalogue& catalogue);

private:
    void ProcessBaseRequest(const json::Node& request, TransportCatalogue& catalogue);
    void ProcessStatRequest(const json::Node& request, TransportCatalogue& catalogue, std::ostream& output_stream);
};

} // namespace transport_catalogue

#include "json_reader.h"

namespace transport_catalogue {

void JsonReader::Read(std::istream& input_stream, TransportCatalogue& catalogue) {
    json::Document doc = json::Load(input_stream);
    const json::Array& base_requests = doc.GetRoot().AsMap().at("base_requests").AsArray();
    const json::Array& stat_requests = doc.GetRoot().AsMap().at("stat_requests").AsArray();

    for (const json::Node& request : base_requests) {
        ProcessBaseRequest(request, catalogue);
    }

    for (const json::Node& request : stat_requests) {
        ProcessStatRequest(request, catalogue, std::cout);
    }
}

void JsonReader::ProcessBaseRequest(const json::Node& request, TransportCatalogue& catalogue) {
    const std::string& type = request.AsMap().at("type").AsString();

    if (type == "Bus") {
        const std::string& bus_name = request.AsMap().at("name").AsString();
        const json::Array& stops = request.AsMap().at("stops").AsArray();
        bool is_roundtrip = request.AsMap().at("is_roundtrip").AsBool();

        std::vector<const Stop*> bus_stops;
        for (const json::Node& stop_name_node : stops) {
            const std::string& stop_name = stop_name_node.AsString();
            const Stop* stop = catalogue.FindStop(stop_name);
            if (!stop) {
                // Handle error: stop not found
            }
            bus_stops.push_back(stop);
        }

        catalogue.AddBus(bus_name, bus_stops, is_roundtrip);
    } else if (type == "Stop") {
        const std::string& stop_name = request.AsMap().at("name").AsString();
        const double latitude = request.AsMap().at("latitude").AsDouble();
        const double longitude = request.AsMap().at("longitude").AsDouble();
        geo::Coordinates coordinates = {latitude, longitude};

        catalogue.AddStop(stop_name, coordinates);

        const json::Dict& road_distances = request.AsMap().at("road_distances").AsMap();
        for (const auto& [other_stop_name, distance] : road_distances) {
            const Stop* other_stop = catalogue.FindStop(other_stop_name);
            if (!other_stop) {
                // Handle error: stop not found
            }
            catalogue.SetDistanceBetweenStops(catalogue.FindStop(stop_name), other_stop, distance);
        }
    } else {
        // Handle error: invalid request type
    }
}

void JsonReader::ProcessStatRequest(const json::Node& request, TransportCatalogue& catalogue, std::ostream& output_stream) {
    const int id = request.AsMap().at("id").AsInt();
    const std::string& type = request.AsMap().at("type").AsString();
    const std::string& name = request.AsMap().at("name").AsString();

    if (type == "Stop") {
        const Stop* stop = catalogue.FindStop(name);
        if (!stop) {
            // Handle error: stop not found
        }
        const std::set<std::string>& buses = catalogue.GetBusesAtStop(name);
        // Print the result to output_stream
    } else if (type == "Bus") {
        const Bus* bus = catalogue.FindBus(name);
        if (!bus) {
            // Handle error: bus not found
        }
        const RouteInfo& route_info = catalogue.RouteInformation(name);
        // Print the result to output_stream
    }
}

} // namespace transport_catalogue