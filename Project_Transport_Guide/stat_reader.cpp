#include <iomanip>
//#include <fstream>
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
            // исправил. Да, это очевидно и логично. *facepalm*
            const RouteInfo route_info = transport_catalogue.RouteInformation(request);
            output << "Bus "s << request << ": " << route_info.all_stops_counter_ << " stops on route, "s
                << route_info.unique_stops_counter_ << " unique stops, "s << std::setprecision(6)
                << route_info.route_length_ << " route length, "s << route_info.curvature_ << " curvature"s << std::endl;
        }
        else {
            output << "Bus "s << request << ": not found"s << std::endl;
        }
    } else if (result[0] == "Stop"s) {
        request = request.substr(5, request.npos);
        if (transport_catalogue.FindStop(request)) {
            output << "Stop "s << request << ": "s;
            auto& buses = transport_catalogue.GetBusesAtStop(request); // исправлено
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

// добавлено
void GetCatalogueStats(std::istream& in, const TransportCatalogue& tansport_catalogue, std::ostream& out) {
    int stat_request_count;
    std::cin >> stat_request_count >> std::ws;

    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        std::getline(in, line);
        ParseAndPrintStat(tansport_catalogue, line, out);
    }
}

} // output

} // transport_catalogue