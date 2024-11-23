#pragma once

// #include <fstream>
#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace transport_catalogue {

namespace output {

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request, std::ostream& output);

void GetCatalogueStats(std::istream& in, const TransportCatalogue& tansport_catalogue, std::ostream& out); // добавлено

} // output

} // transport_catalogue