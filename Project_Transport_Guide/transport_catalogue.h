#pragma once

#include <deque>
// #include <functional> // ?
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "geo.h"

namespace transport_catalogue {

struct Stop {
    std::string stop_name_;
    transport_catalogue::geo::Coordinates coordinates_;
    std::set<std::string> buses_at_stop_;
};

struct Bus {
    std::string bus_number_;
    std::vector<const Stop*> bus_stops_;
};

struct RouteInfo {
    size_t all_stops_counter_;
    size_t unique_stops_counter_;
    double route_length_;
    double curvature_;
};

class TransportCatalogue {
public:
    struct Hasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& hash_value) const {
            size_t hash_a = std::hash<const void*>{}(hash_value.first);
            size_t hash_b = std::hash<const void*>{}(hash_value.second);
            return hash_a ^ hash_b << 1;
        }
    };

    void AddStop(const std::string_view stop_name, transport_catalogue::geo::Coordinates& coordinates);
    void AddBus(const std::string_view bus_number, const std::vector<const Stop*>& bus_stops); // страно, 
    const Stop* FindStop(const std::string_view stop_name) const; // у меня отступ 4 пробела - 1 табуляция 
    const Bus* FindBus(const std::string_view bus_number) const; // на всех строчках с 34 по 37. Перепроверил и пересохранил.
    //
    const RouteInfo RouteInformation(const std::string_view bus_number) const;
    const std::set<std::string>& GetBusesAtStop(std::string_view stop_name) const;
    void SetDistanceBetweenStops(const Stop* point_a, const Stop* point_b, const int distance);
    int GetDistanceBetweenStops(const Stop* point_a, const Stop* point_b) const;

private:
    std::deque<Stop> all_stops_; // сделал табуляцию 1 раз, но вроде и так всё было ровно.
    std::deque<Bus> all_buses_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, Hasher> distances_between_stops_;

    size_t UniqueStopsCount(const std::string_view bus_number) const; // перенесено из public
};

} // transport_catalogue