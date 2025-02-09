/*
Вам нужно изменить TransportRouter так, чтобы он возвращал детали маршрута непосредственно,
а не позволял доступ к внутреннему графу. Затем нужно обновить RequestHandler,
чтобы он использовал этот измененный интерфейс. Вот шаги и изменения в коде:

1. Изменение TransportRouter:
Удалите метод GetGraph() из transport_router.h и transport_router.cpp.
Вместо этого измените FindRoute так, чтобы он возвращал структуру,
содержащую всю необходимую информацию о маршруте в нужном формате.
*/
// transport_router.h
#pragma once

// ... (includes)

namespace transport_router {

// ... (TransportRouterSettings)

struct RouteItem {
    std::string type; // "Wait" или "Bus"
    std::string stop_name; // Для "Wait"
    std::string bus; // Для "Bus"
    int span_count; // Для "Bus"
    double time;
};

struct Route {
    double total_time = 0.0;
    std::vector<RouteItem> items;
};

class TransportRouter {
public:
    // ... (constructor)

    std::optional<Route> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

private:
    // ... (members)

    const graph::DirectedWeightedGraph<double>& BuildGraph(const transport_catalogue::TransportCatalogue& catalogue);
};

} // namespace transport_router


// transport_router.cpp
#include <string>
#include <algorithm> // для std::reverse

// ... (includes)

namespace transport_router {

// ... (BuildGraph)

std::optional<Route> TransportRouter::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
    auto route_info = router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
    if (!route_info) {
        return std::nullopt;
    }

    Route route;
    route.total_time = route_info->weight;

    std::vector<graph::EdgeId> edges = route_info->edges;
    if (!edges.empty()) {
        std::reverse(edges.begin(), edges.end()); // Восстанавливаем порядок ребер

        for (graph::EdgeId edge_id : edges) {
            const auto& edge = graph_.GetEdge(edge_id);
            RouteItem item;
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

// ... остальной код
} // namespace transport_router

/////////////////////////////////////////////////////
/*
2. Изменение RequestHandler:

Измените RequestHandler, чтобы он использовал новый метод FindRoute
и формировал JSON ответ в требуемом формате.
*/
// request_handler.h
#pragma once
#include "transport_router.h"
// ...

class RequestHandler {
public:
    // ...

    std::optional<transport_router::Route> GetRouteInfo(const std::string_view stop_from, const std::string_view stop_to) const;

private:
    const transport_router::TransportRouter& router_;
    // ...
};

// request_handler.cpp
#include "request_handler.h"
// ...

std::optional<transport_router::Route> RequestHandler::GetRouteInfo(const std::string_view stop_from, const std::string_view stop_to) const {
    return router_.FindRoute(stop_from, stop_to);
}

// ...

// request_handler.cpp (изменения внутри ProcessStatRequest)
else if (type == "Route"s) {
    const std::string_view stop_from = request.AsDict().at("from"s).AsString();
    const std::string_view stop_to = request.AsDict().at("to"s).AsString();
    const std::optional<transport_router::Route> route_info = GetRouteInfo(stop_from, stop_to); // Используем новый метод

    if (!route_info) {
        response = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("error_message"s).Value("not found"s)
            .EndDict()
        .Build().AsDict();
    } else {
        json::Array items;
        for (const auto& item : route_info->items) {
            json::Dict route_item;
            route_item["type"s] = item.type;
            if (item.type == "Wait") {
                route_item["stop_name"s] = item.stop_name;
                route_item["time"s] = item.time;
            } else {
                route_item["bus"s] = item.bus;
                route_item["span_count"s] = item.span_count;
                route_item["time"s] = item.time;
            }
            items.push_back(route_item);
        }

        response = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("total_time"s).Value(route_info->total_time)
                .Key("items"s).Value(items)
            .EndDict()
        .Build().AsDict();
    }
}

// ... (остальной код request_handler.cpp)
/*
Ключевые изменения:

TransportRouter::FindRoute теперь возвращает структуру Route с элементами RouteItem,
содержащими информацию в нужном формате.
RequestHandler вызывает TransportRouter::FindRoute и формирует JSON ответ,
используя новую структуру.
Метод GetGraph() удален из TransportRouter.
В RequestHandler::ProcessStatRequest изменена логика обработки запроса Route
для использования новой структуры Route и формирования JSON ответа в требуемом формате.
После этих изменений RequestHandler сможет получать всю необходимую информацию
о маршруте и формировать JSON ответ в точности с требуемым форматом.
Обязательно протестируйте эти изменения, чтобы убедиться, что все работает корректно.
*/