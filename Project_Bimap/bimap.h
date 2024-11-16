#pragma once

#include <memory>
#include <optional>
#include <string_view>

class BiMap {
public:

    BiMap();
    ~BiMap();

    BiMap(const BiMap& other);               // Копирующий конструктор
    BiMap(BiMap&& other) noexcept;           // Перемещающий конструктор
    BiMap& operator=(const BiMap& other);    // Копирующее присваивание
    BiMap& operator=(BiMap&& other) noexcept;// Перемещающее присваивание

    /**
     * Добавляет в словарь пару "ключ-значение".
     * В случае успеха возвращает true.
     * Возвращает false, если ключ или значение были ранее добавлены.
     * 
     * Метод обеспечивает строгую гарантию безопасности исключений
     */
    bool Add(std::string_view key, std::string_view value);

    /**
     * Возвращает значение, связанное с ключом, либо nullopt, если такого ключа нет
     */
    std::optional<std::string_view> FindValue(std::string_view key) const noexcept;

    /**
     * Возвращает ключ, связанный с value, либо nullopt, если такого значения нет
     */
    std::optional<std::string_view> FindKey(std::string_view value) const noexcept;

private:
    /* Класс должен использовать идиому Pimpl, поддерживать копирование и перемещение */
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

