#pragma once

#include "json_reader.h"
#include "router.h"
#include "transport_catalogue.h"

#include <memory>
#include <string>

namespace transport_router {

using namespace std::literals;

class Router {
public:

    Router(const JsonReader& input_doc, const transport_catalogue::TransportCatalogue& catalogue) : input_doc_(input_doc) {
		json::Dict temp_dict = GetRoutingSettings();
		bus_wait_time_ = temp_dict.at("bus_wait_time"s).AsInt();
		bus_velocity_ = temp_dict.at("bus_velocity"s).AsDouble();
		BuildGraph(catalogue);
	}

	const graph::DirectedWeightedGraph<double>& BuildGraph(const transport_catalogue::TransportCatalogue& catalogue);
	const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
	const graph::DirectedWeightedGraph<double>& GetGraph() const;

private:
	int bus_wait_time_ = 0;
	double bus_velocity_ = 0.0;

    const JsonReader& input_doc_;
	graph::DirectedWeightedGraph<double> graph_;
	std::map<std::string, graph::VertexId> stop_ids_;
	std::unique_ptr<graph::Router<double>> router_;
    const json::Dict& GetRoutingSettings() const;
};

}