/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "transport_catalogue.h"
#include "json.h"

class JsonReader {
public:
    JsonReader(std::istream& input) : input_doc_(json::Load(input)) {
    }

    void FillCatalogueWithRequests(transport_catalogue::TransportCatalogue& catalogue);

    const json::Document& GetInputDoc() const;

private:
    json::Document input_doc_;
    
    void ProcessBaseRequest(const json::Node& request
                          , const std::string& type
                          , transport_catalogue::TransportCatalogue& catalogue
                          , std::vector<transport_catalogue::Distance>& distances);
};