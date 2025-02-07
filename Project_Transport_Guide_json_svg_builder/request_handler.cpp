/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Изменения претерпели блоки формирования узлов Node типа Dict; они заменены на методы из json_builder
#include <set>
#include <stdexcept>
#include <string>
#include <sstream>

#include "domain.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "json_builder.h"

using namespace std::literals;

const std::set<std::string>& RequestHandler::GetBusesAtStop(const std::string_view& stop_name) const {
    return catalogue_.GetStopNameToStop().at(stop_name)->buses_at_stop_;
}

void RequestHandler::ProcessStatRequest(const json::Node& request, std::ostream& output_stream) {
    const int id = request.AsDict().at("id"s).AsInt();
    const std::string& type = request.AsDict().at("type"s).AsString();
    json::Dict response;
    //response["request_id"s] = id;

    if (type == "Map"s) {
        std::ostringstream map_in_svg_format;
        renderer_.RenderMap().Render(map_in_svg_format);
        //response["map"] = map_in_svg_format.str();
        response = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("map"s).Value(map_in_svg_format.str())
            .EndDict()
        .Build().AsDict();
    } else {
        const std::string& name = request.AsDict().at("name"s).AsString();
        if (type == "Stop"s) {
            const transport_catalogue::Stop* stop = catalogue_.FindStop(name);
            if (stop != nullptr) {
                const std::set<std::string>& buses = GetBusesAtStop(name);
                //response["buses"s] = json::Array(buses.begin(), buses.end());
                response = json::Builder{}
                    .StartDict()
                        .Key("request_id"s).Value(id)
                        .Key("buses"s).Value(json::Array(buses.begin(), buses.end()))
                    .EndDict()
                .Build().AsDict();
            } else {
                //response["error_message"s] = "not found"s;
                response = json::Builder{}
                    .StartDict()
                        .Key("request_id"s).Value(id)
                        .Key("error_message"s).Value("not found"s)
                    .EndDict()
                .Build().AsDict();
            }
        } else if (type == "Bus"s) {
            const transport_catalogue::Bus* bus = catalogue_.FindBus(name);
            if (bus != nullptr) {
                const transport_catalogue::RouteInfo& route_info = catalogue_.RouteInformation(name);
                //response["curvature"s] = route_info.curvature_;
                //response["route_length"s] = route_info.route_length_;
                //response["stop_count"s] = static_cast<int>(route_info.all_stops_counter_);
                //response["unique_stop_count"s] = static_cast<int>(route_info.unique_stops_counter_);
                response = json::Builder{}
                    .StartDict()
                        .Key("request_id"s).Value(id)
                        .Key("curvature"s).Value(route_info.curvature_)
                        .Key("route_length"s).Value(route_info.route_length_)
                        .Key("stop_count"s).Value(static_cast<int>(route_info.all_stops_counter_))
                        .Key("unique_stop_count"s).Value(static_cast<int>(route_info.unique_stops_counter_))
                    .EndDict()
                .Build().AsDict();
            } else {
                //response["error_message"s] = "not found"s;
                response = json::Builder{}
                    .StartDict()
                        .Key("request_id"s).Value(id)
                        .Key("error_message"s).Value("not found"s)
                    .EndDict()
                .Build().AsDict();
            }
        } else {
            // Обработка ошибки invalid request type
            //response["error_message"s] = "invalid request type"s;
            response = json::Builder{}
                .StartDict()
                    .Key("request_id"s).Value(id)
                    .Key("error_message"s).Value("invalid request type"s)
                    .EndDict()
            .Build().AsDict();
        }
    }
    json::PrintContext context{output_stream, 4, 4};

    json::PrintValue(response, context);
}

void RequestHandler::PrintStatRequest() {
    const json::Array& stat_requests = input_doc_.GetInputDoc().GetRoot().AsDict().at("stat_requests"s).AsArray();
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