#include <iostream>
#include <fstream>
#include <string>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
//#include "log_duration.h"

using namespace std;
using namespace transport_catalogue_app;

int main() {
    core::TransportCatalogue catalogue;
    io::InputReader reader;

    int base_request_count;
    cin >> base_request_count >> ws;
    //std::ifstream in("input.txt");
    for (int i = 0; i < base_request_count; ++i) {
        string line;
        getline(cin, line);
        reader.ParseLine(line);
    }

    reader.ApplyCommands(catalogue);

    int stat_request_count;
    cin >> stat_request_count >> ws;
    // std::ifstream stats("stats.txt");
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(cin, line);
        io::ParseAndPrintStat(catalogue, line, cout);
    }

int i;
std::cin >> i;
}