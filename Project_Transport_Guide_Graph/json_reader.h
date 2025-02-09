#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "json.h"
#include "transport_catalogue.h"
#include "transport_router.h"

const double KM_PER_HOUR_TO_METER_PER_MINUTE_TRANSFER = 1000.0 / 60.0; // вынес как отдельную именованную константу

class JsonReader {
public:
    JsonReader(std::istream& input) : input_doc_(json::Load(input)) {
    }

    void FillCatalogueWithRequests(transport_catalogue::TransportCatalogue& catalogue);

    const json::Document& GetInputDoc() const;

    const transport_router::TransportRouterSettings GetRoutingSettings() const; // метод перенёс сюда из transport_router

private:
    json::Document input_doc_;
    
    void ProcessBaseRequest(const json::Node& request
                          , const std::string& type
                          , transport_catalogue::TransportCatalogue& catalogue
                          , std::vector<transport_catalogue::Distance>& distances);
};