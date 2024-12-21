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
#include <sstream>

#include "json_reader.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "svg.h"

using namespace std;

int main() {

    transport_catalogue::TransportCatalogue catalogue;
    JsonReader json_document(std::cin);
    json_document.FillCatalogueWithRequests(catalogue);
    MapRenderer map_renderer(catalogue, json_document);
    RequestHandler request_handler(catalogue, json_document, map_renderer);
    request_handler.PrintStatRequest();
    //request_handler.RenderMap().Render(std::cout);
}