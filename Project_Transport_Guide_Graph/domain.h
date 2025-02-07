/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
// без изменений с прошлого спринта
#pragma once

#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"

namespace transport_catalogue {

struct Stop {
    std::string stop_name_;
    geo::Coordinates coordinates_;
    std::set<std::string> buses_at_stop_;
};

struct Bus {
    std::string bus_number_;
    std::vector<const Stop*> bus_stops_;
    bool is_roundtrip;
};

struct RouteInform {
    size_t all_stops_counter_;
    size_t unique_stops_counter_;
    double route_length_;
    double curvature_;
};

struct Distance {
    std::string point_a_;
    std::string point_b_;
    int distance_;
};

}