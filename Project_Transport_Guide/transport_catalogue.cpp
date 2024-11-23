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

void TransportCatalogue::AddStop(const std::string_view stop_name, transport_catalogue::geo::Coordinates& coordinates) {
    all_stops_.push_back({std::string(stop_name), coordinates, {}});
    stopname_to_stop_[all_stops_.back().stop_name_] = &all_stops_.back();
}

void TransportCatalogue::AddBus(const std::string_view bus_number, const std::vector<const Stop*>& bus_stops) {
    all_buses_.push_back({std::string(bus_number), bus_stops});
    busname_to_bus_[all_buses_.back().bus_number_] = &all_buses_.back();

    for (const auto& stop_ptr : bus_stops) {
        /*изменил тип контейнера
        stopname_to_stop_ unordered_map<std::string_view, const Stop*>
        на                unordered_map<std::string_view, Stop*>
        теперь можно обращаться по указателю напрямую к объекту Stop */
        stopname_to_stop_.at(stop_ptr->stop_name_)->buses_at_stop_.insert(std::string(bus_number));
    }
}

const Stop* TransportCatalogue::FindStop(const std::string_view stop_name) const {
    auto stop_ptr = stopname_to_stop_.find(stop_name); // исправлено
    if (stop_ptr != stopname_to_stop_.end()) {
        return stop_ptr->second;
    } else {
        return nullptr;
    }
}

const Bus* TransportCatalogue::FindBus(const std::string_view bus_number) const {
    auto bus_ptr = busname_to_bus_.find(bus_number); // исправлено
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

const RouteInfo TransportCatalogue::RouteInformation(const std::string_view bus_number) const {
    RouteInfo route_info = {};
    const Bus* bus = FindBus(bus_number);

    if (bus == nullptr) {
        throw std::invalid_argument("Bus is not found"s);
    }

    route_info.all_stops_counter_ = bus->bus_stops_.size();

    double geo_route_length = 0.0;
    double route_length = 0.0;

    for (size_t it = 0; it < bus->bus_stops_.size(); ++it) {
        const Stop* point_a = bus->bus_stops_[it];
        const Stop* point_b = bus->bus_stops_[it + 1];
        if (it == bus->bus_stops_.size() - 1) {
            point_b = bus->bus_stops_[0];
        }
        route_length += GetDistanceBetweenStops(point_a, point_b);
        geo_route_length += ComputeDistance(point_a->coordinates_, point_b->coordinates_);
    }

    route_info.unique_stops_counter_ = UniqueStopsCount(bus_number);
    route_info.route_length_ = route_length;
    route_info.curvature_ = route_length / geo_route_length;

    return route_info;
}

const std::set<std::string>& TransportCatalogue::GetBusesAtStop(std::string_view stop_name) const {
    return stopname_to_stop_.at(stop_name)->buses_at_stop_; // исправлено
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