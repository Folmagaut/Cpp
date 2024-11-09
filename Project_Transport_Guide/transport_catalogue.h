#pragma once

#include <deque>
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
    //bool is_circle_;
};

struct RouteInfo {
    size_t all_stops_counter_;
    size_t unique_stops_counter_;
    double route_length_;
};

class TransportCatalogue {
public:
    void AddStop(const std::string_view stop_name, transport_catalogue::geo::Coordinates& coordinates);
	void AddBus(const std::string_view bus_number, const std::vector<const Stop*>& bus_stops/* , bool is_circle */);
    const Stop* FindStop(const std::string_view stop_name) const;
	const Bus* FindBus(const std::string_view bus_number) const;
	size_t UniqueStopsCount(const std::string_view bus_number) const;
    const RouteInfo RouteInformation(const std::string_view bus_number) const;
    const std::set<std::string> GetBusesAtStop(std::string_view stop_name) const;

private:
	std::deque<Stop> all_stops_;
    std::deque<Bus> all_buses_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
};

} // transport_catalogue