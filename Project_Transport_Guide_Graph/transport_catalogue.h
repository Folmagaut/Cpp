// без изменений с прошлого спринта
#pragma once

#include <deque>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "geo.h"
#include "domain.h"

namespace transport_catalogue {

class TransportCatalogue {
public:
    struct Hasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& hash_value) const {
            size_t hash_a = std::hash<const void*>{}(hash_value.first);
            size_t hash_b = std::hash<const void*>{}(hash_value.second);
            return hash_a ^ hash_b << 1;
        }
    };

    void AddStop(const std::string_view stop_name, geo::Coordinates& coordinates);
    void AddBus(const std::string_view bus_number, const std::vector<const Stop*>& bus_stops , bool is_roundtrip);
    const Stop* FindStop(const std::string_view stop_name) const;
    const Bus* FindBus(const std::string_view bus_number) const;
    void SetDistanceBetweenStops(const Stop* point_a, const Stop* point_b, const int distance);
    int GetDistanceBetweenStops(const Stop* point_a, const Stop* point_b) const;
    size_t UniqueStopsCount(const std::string_view bus_number) const;
    const std::unordered_map<std::string_view, Stop*>& GetStopNameToStop() const;
    const std::deque<Stop>& GetAllStops() const;
    const std::map<std::string_view, const Bus*> GetSortedAllBuses() const;
    const std::map<std::string_view, const Stop*> GetSortedAllStops() const;

    // Вообще, этот метод изначально и был здесь в приватной области
    // но в прекоде задания сразу предложили перенести в RequestHandler этот метод
    // так как он участвует в формировании ответов на запрос;
    // единственное - мне пришлось его убрать его из приватной области,
    // т. к. он нужен в RequestHandler
    const RouteInform RouteInformation(const std::string_view& bus_number) const;

private:
    std::deque<Stop> all_stops_;
    std::deque<Bus> all_buses_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, Hasher> distances_between_stops_;

};

} // transport_catalogue