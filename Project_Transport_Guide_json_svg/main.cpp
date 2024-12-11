    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"

using namespace std;

int main() {

    transport_catalogue::TransportCatalogue catalogue;
    //ifstream input("input_old.txt");
    istringstream str(R"(
  {"base_requests": [{"type": "Stop", "name": "A", "latitude": 0.5, "longitude": -1, "road_distances": {"B": 100000}}, {"type": "Stop", "name": "B", "latitude": 0, "longitude": -1.1, "road_distances": {}}, {"type": "Bus", "name": "256", "stops": ["B", "A"], "is_roundtrip": false}], "stat_requests": [{"id": 118087048, "type": "Bus", "name": "256"}, {"id": 1666915729, "type": "Stop", "name": "A"}, {"id": 1547120284, "type": "Stop", "name": "B"}, {"id": 1042626800, "type": "Stop", "name": "C"}]}
)"s);
    JsonReader json_document(str/* std::cin */);
    json_document.FillCatalogueWithRequests(catalogue);
    //transport_catalogue::input::FillCatalogueWithRequests(cin, catalogue);

    //transport_catalogue::output::GetCatalogueStats(cin, catalogue, cout);
    //const auto& stat_requests = json_doc.GetStatRequests();
    //rh.ProcessRequests(stat_requests);
}