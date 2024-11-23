#include <iostream>
#include <string>

#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport_catalogue::TransportCatalogue catalogue;

    transport_catalogue::input::FillCatalogueWithRequests(cin, catalogue); // перенёс всё в input_reader

    transport_catalogue::output::GetCatalogueStats(cin, catalogue, cout); // перенёс всё в output_reader
}