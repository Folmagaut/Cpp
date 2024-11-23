/* #include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <utility>

#include "geo.h"
#include "input_reader.h"

using namespace std::literals;

namespace transport_catalogue {

namespace detail {

std::string_view Trim(std::string_view string);

std::pair<std::string, std::string> ParseDistances(std::string id, std::string_view str) {
    //auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');
    auto not_space2 = str.find_first_not_of(' ', comma + 1);
    auto comma2 = str.find(',', not_space2);
    std::string distances = std::string(str.substr(comma2 + 2));
    return std::make_pair(id, distances);
}

//
// * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
//
transport_catalogue::geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);
    auto comma2 = str.find(',', not_space2);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2, comma2 - not_space2)));

    //std::string dist = std::string(str.substr(comma2 + 2));

    //ParseDistances(id, dist);

    return {lat, lng};
}

//
// Удаляет пробелы в начале и конце строки
//
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

//
// Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
//
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }
    return result;
}

//
// Парсит маршрут.
// Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
// Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
//
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

} // detail

namespace input {

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

// void InputReader::AddDistance(std::pair<std::string, std::string> id_dist_str) {
//        distances_.push_back(id_dist_str);
//}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
    // Реализуйте метод самостоятельно
    std::vector<CommandDescription> buses = {};
    std::vector<std::pair<std::string_view, std::string_view>> distances_;

    for (auto& query : commands_) {
        if (query.command == "Bus"s) {
            buses.push_back(std::move(query));
        } else {
            transport_catalogue::geo::Coordinates lat_lng = transport_catalogue::detail::ParseCoordinates(query.description);
            catalogue.AddStop(query.id, lat_lng);
            distances_.push_back(std::move(transport_catalogue::detail::ParseDistances(query.id, query.description)));
        }
    }
    //distances_ = transport_catalogue::detail::ParseDistances(query.description);
    for (auto& query : buses) {
        std::vector<std::string_view> bus_route = transport_catalogue::detail::ParseRoute(query.description);
        std::vector<const Stop*> bus_stops = {};
        for (auto& bus_stop : bus_route) {
            bus_stops.push_back(std::move(catalogue.FindStop(bus_stop)));
        }
        catalogue.AddBus(query.id, bus_stops);
    }
}

// добавлено
void FillCatalogueWithRequests( std::istream& in, TransportCatalogue& catalogue) {
    int base_request_count = 1;
    //in >> base_request_count >> std::ws;

    InputReader reader;
    for (int i = 0; i < base_request_count; ++i) {
        //std::string line;
        //std::getline(in, line);
        reader.ParseLine("Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino"s);
    }
    reader.ApplyCommands(catalogue);
}

} // input

} // transport_catalogue

std::istringstream iss(dist);
    //std::vector<std::pair<std::string, std::string>> distances_;
    std::string distance;
    while (getline(iss, distance, ',')) {
        distance = Trim(distance);
        //distances_.push_back(std::make_pair(id, distance));
    } */