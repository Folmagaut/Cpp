#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>
#include <string>

namespace transport_router {

using namespace std::literals;
// структуру создал, формирование настроек перенёс в метод класса JsonReader
struct TransportRouterSettings {
	int bus_wait_time_ = 0; // ожидание в минутах
	double bus_speed_ = 0.0; // скорость в м/мин
};

// две новые структуры, для возврщения результатов в FindRoute
struct RouteItem {
    std::string type; // "Wait" или "Bus"
    std::string stop_name; // для "Wait"
    std::string bus; // для "Bus"
    int span_count; // для "Bus"
    double time;
};

struct Route {
    double total_time = 0.0;
    std::vector<RouteItem> items;
};

class TransportRouter {
public:
	// router_settings передаю по значению, так как объект небольшой - 16 байт
	TransportRouter(const transport_catalogue::TransportCatalogue& catalogue, const TransportRouterSettings router_settings)
	: bus_wait_time_(router_settings.bus_wait_time_)
	, bus_speed_(router_settings.bus_speed_) {
		BuildGraph(catalogue);
	}

	std::optional<Route> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const; // новый метод, старый закоментировал

	//const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
	// убрал
	//const graph::DirectedWeightedGraph<double>& GetGraph() const;

private:
	int bus_wait_time_ = 0; // ожидание в минутах
	double bus_speed_ = 0.0; // скорость в м/мин
	graph::DirectedWeightedGraph<double> graph_;
	std::map<std::string, graph::VertexId> stop_ids_;
	std::unique_ptr<graph::Router<double>> router_;

	const graph::DirectedWeightedGraph<double>& BuildGraph(const transport_catalogue::TransportCatalogue& catalogue); // перенёс в private
};

}