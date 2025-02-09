#include <iostream>
//#include <sstream>
#include <string>

#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "svg.h"
#include "transport_catalogue.h"
#include "transport_router.h"

using namespace std;

int main() {

    transport_catalogue::TransportCatalogue catalogue;
    
    JsonReader json_document(std::cin);

    json_document.FillCatalogueWithRequests(catalogue);

    MapRenderer map_renderer(catalogue, json_document);

    const transport_router::TransportRouterSettings router_settings = json_document.GetRoutingSettings();

    transport_router::TransportRouter router(catalogue, router_settings);

    RequestHandler request_handler(catalogue, json_document, map_renderer, router);
    request_handler.PrintStatRequest();
    //request_handler.RenderMap().Render(std::cout);
}