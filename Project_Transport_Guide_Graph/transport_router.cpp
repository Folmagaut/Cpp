#include <string>
#include "transport_router.h"

namespace transport_router {

using namespace std::literals;

const graph::DirectedWeightedGraph<double>& TransportRouter::BuildGraph(const transport_catalogue::TransportCatalogue& catalogue) {
	const std::map<std::string_view, const transport_catalogue::Stop*>& all_stops = catalogue.GetSortedAllStops();
	const std::map<std::string_view, const transport_catalogue::Bus*>& all_buses = catalogue.GetSortedAllBuses();
	graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
    std::map<std::string, graph::VertexId> stop_ids;
    graph::VertexId vertex_id = 0;

    for (const auto& [stop_name, stop_info] : all_stops) {
        stop_ids[stop_info->stop_name_] = vertex_id;
        stops_graph.AddEdge({
                stop_info->stop_name_,
                0,
                vertex_id,
                ++vertex_id,
                static_cast<double>(bus_wait_time_)
            });
        ++vertex_id;
    }
    stop_ids_ = std::move(stop_ids);

    for (auto it = all_buses.begin(); it != all_buses.end(); ++it) {
        const transport_catalogue::Bus* bus_info = it->second;
        const std::vector<const transport_catalogue::Stop*>& stops = bus_info->bus_stops_;
        size_t stops_count = stops.size();
        for (size_t i = 0; i < stops_count; ++i) {
            for (size_t j = i + 1; j < stops_count; ++j) {
                const transport_catalogue::Stop* stop_from = stops[i];
                const transport_catalogue::Stop* stop_to = stops[j];
                int dist_sum_forward = 0;
                int dist_sum_backward = 0;
                for (size_t k = i + 1; k <= j; ++k) {
                    dist_sum_forward += catalogue.GetDistanceBetweenStops(stops[k - 1], stops[k]);
                    dist_sum_backward += catalogue.GetDistanceBetweenStops(stops[k], stops[k - 1]);                    }
                    stops_graph.AddEdge({bus_info->bus_number_, j - i, stop_ids_.at(stop_from->stop_name_) + 1, stop_ids_.at(stop_to->stop_name_),
                                        static_cast<double>(dist_sum_forward) / bus_speed_});

                if (!bus_info->is_roundtrip) {
                    stops_graph.AddEdge({bus_info->bus_number_, j - i, stop_ids_.at(stop_to->stop_name_) + 1, stop_ids_.at(stop_from->stop_name_),
                                        static_cast<double>(dist_sum_backward) / bus_speed_});
                    }
                }
            }
        }
    graph_ = std::move(stops_graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);

    return graph_;
}

// переписал метод
std::optional<Route> TransportRouter::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
	auto route_info = router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
    if (!route_info) {
        return std::nullopt;
    }

    Route route; // общая структура
    route.total_time = route_info->weight;

    std::vector<graph::EdgeId> edges = route_info->edges;
    if (!edges.empty()) {
        std::reverse(edges.begin(), edges.end());

        for (graph::EdgeId edge_id : edges) {
            const auto& edge = graph_.GetEdge(edge_id);
            RouteItem item; // заполняем каждый элемент
            if (edge.span_count == 0) {
                item.type = "Wait";
                item.stop_name = edge.name_of_bus;
                item.time = edge.weight;
            } else {
                item.type = "Bus";
                item.bus = edge.name_of_bus;
                item.span_count = static_cast<int>(edge.span_count);
                item.time = edge.weight;
            }
            route.items.push_back(item);
        }
    }
    return route;
}

/* const std::optional<graph::Router<double>::RouteInfo> TransportRouter::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
	return router_->BuildRoute(stop_ids_.at(std::string(stop_from)),stop_ids_.at(std::string(stop_to)));
} */

/* const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
	return graph_;
} */

}