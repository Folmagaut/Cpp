/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)

// без изменений с прошлого спринта
#include <optional>
#include <set>
#include <string>
#include <string_view>

#include "graph.h"
#include "json.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"


class RequestHandler {
public:

    RequestHandler(const transport_catalogue::TransportCatalogue& catalogue, const JsonReader& input_doc, const MapRenderer& renderer, const transport_catalogue::Router& router)
        : catalogue_(catalogue), input_doc_(input_doc), renderer_(renderer), router_(router) {
    }

    void PrintStatRequest();

    

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const transport_catalogue::TransportCatalogue& catalogue_;
    const JsonReader& input_doc_;
    const MapRenderer& renderer_;
    const transport_catalogue::Router& router_;

    // Возвращает маршруты, проходящие через
    const std::set<std::string>& GetBusesAtStop(const std::string_view& stop_name) const;
    const std::optional<graph::Router<double>::RouteInfo> GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const;
    const graph::DirectedWeightedGraph<double>& GetRouterGraph() const;
    
    void ProcessStatRequest(const json::Node& request, std::ostream& output_stream);
};