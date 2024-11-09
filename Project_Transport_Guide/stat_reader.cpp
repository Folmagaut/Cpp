#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>

#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

namespace transport_catalogue {

namespace output {

using namespace std::literals;

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    // Реализуйте самостоятельно
    std::vector<std::string_view> result = transport_catalogue::detail::Split(request, ' ');
    
    if (result[0] == "Bus"s) {
        request = request.substr(4, request.npos);
        if (transport_catalogue.FindBus(request)) {
            output << "Bus "s << request << ": " << transport_catalogue.RouteInformation(request).all_stops_counter_ << " stops on route, "s
                << transport_catalogue.RouteInformation(request).unique_stops_counter_ << " unique stops, "s << std::setprecision(6)
                << transport_catalogue.RouteInformation(request).route_length_ << " route length"s << std::endl;
        }
        else {
            output << "Bus "s << request << ": not found"s << std::endl;
        }
    } else if (result[0] == "Stop"s) {
        request = request.substr(5, request.npos);
        if (transport_catalogue.FindStop(request)) {
            output << "Stop "s << request << ": "s;
            auto buses = transport_catalogue.GetBusesAtStop(request);
            if (!buses.empty()) {
                output << "buses "s;
                for (const auto& bus : buses) {
                    output << bus << " "s;
                }
                output << std::endl;
            } else {
                output << "no buses"s << std::endl;
            }
        } else {
            output << "Stop "s << request << ": not found"s << std::endl;
        }
    }
}

} // output

} // transport_catalogue