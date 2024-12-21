#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "transport_catalogue.h"
#include "geo.h"

using namespace std::literals;

namespace transport_catalogue {

void TransportCatalogue::AddStop(const std::string_view stop_name, geo::Coordinates& coordinates) {
    all_stops_.push_back({std::string(stop_name), coordinates, {}});
    stopname_to_stop_[all_stops_.back().stop_name_] = &all_stops_.back();
}

void TransportCatalogue::AddBus(const std::string_view bus_number, const std::vector<const Stop*>& bus_stops , bool is_roundtrip) {
    all_buses_.push_back({std::string(bus_number), bus_stops, is_roundtrip});
    busname_to_bus_[all_buses_.back().bus_number_] = &all_buses_.back();

    for (const auto& stop_ptr : bus_stops) {
        stopname_to_stop_.at(stop_ptr->stop_name_)->buses_at_stop_.insert(std::string(bus_number));
    }
}

const Stop* TransportCatalogue::FindStop(const std::string_view stop_name) const {
    auto stop_ptr = stopname_to_stop_.find(stop_name);
    if (stop_ptr != stopname_to_stop_.end()) {
        return stop_ptr->second;
    } else {
        return nullptr;
    }
}

const Bus* TransportCatalogue::FindBus(const std::string_view bus_number) const {
    auto bus_ptr = busname_to_bus_.find(bus_number);
    if (bus_ptr != busname_to_bus_.end()) {
        return bus_ptr->second;
    } else {
        return nullptr;
    }
}

size_t TransportCatalogue::UniqueStopsCount(const std::string_view bus_number) const {
    std::unordered_set<std::string_view> unique_stops;
    for (const auto& stop : busname_to_bus_.at(bus_number)->bus_stops_) {
        unique_stops.insert(stop->stop_name_);
    }
    return unique_stops.size();
}

const std::unordered_map<std::string_view, Stop*>& TransportCatalogue::GetStopNameToStop() const {
    return stopname_to_stop_;
}

const std::deque<Stop>& TransportCatalogue::GetAllStops() const {
    return all_stops_;
}

const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedAllBuses() const {
    std::map<std::string_view, const Bus*> buses_sorted;
    for (const auto& bus : busname_to_bus_) {
        buses_sorted.emplace(bus);
    }
    return buses_sorted;
}

const std::map<std::string_view, const Stop*> TransportCatalogue::GetSortedAllStops() const {
    std::map<std::string_view, const Stop*> stops_sorted;
    for (const auto& stop : stopname_to_stop_) {
        stops_sorted.emplace(stop);
    }
    return stops_sorted;
}

void TransportCatalogue::SetDistanceBetweenStops(const Stop* point_a, const Stop* point_b, const int distance) {
    distances_between_stops_[{point_a, point_b}] = distance;
}

int TransportCatalogue::GetDistanceBetweenStops(const Stop* point_a, const Stop* point_b) const {
    if (distances_between_stops_.find({point_a, point_b}) != nullptr) {
        return distances_between_stops_.at({point_a, point_b});
    } else if (distances_between_stops_.find({point_b, point_a}) != nullptr) {
        return distances_between_stops_.at({point_b, point_a});
    } else {
        return 0;
    }
}

} // transport_catalogue