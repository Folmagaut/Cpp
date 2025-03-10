#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "transport_catalogue.h"

namespace transport_catalogue {

namespace detail {

std::vector<std::string_view> Split(std::string_view string, char delim);

} // detail

namespace input {

struct Distance {
    std::string point_a_;
    std::string point_b_;
    int distance_;
};

struct CommandDescription {
    // Определяет, задана ли команда (поле command непустое)
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды
};

class InputReader {
public:
    /**
     * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
     */
    void ParseLine(std::string_view line);

    /**
     * Наполняет данными транспортный справочник, используя команды из commands_
     */
    void ApplyCommands(TransportCatalogue& catalogue) const;

private:
    std::vector<CommandDescription> commands_;
};

void FillCatalogueWithRequests(std::istream& in, TransportCatalogue& catalogue); // добавлено

} // input

} // transport_catalogue