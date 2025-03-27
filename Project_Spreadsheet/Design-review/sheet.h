#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    struct PositionHash {
        size_t operator()(const Position& pos) const {
            return std::hash<std::string>()(pos.ToString());
        }
    };
    
    std::vector<std::vector<std::unique_ptr<Cell>>> cells_;
    Size printable_size_ = {0, 0};
    std::unordered_map<Position, std::unordered_set<Position>, PositionHash> dependencies_; // Граф зависимостей

    void ResizeCells(Position pos);
    void UpdatePrintableSize(Position pos);
    void RecalculatePrintableSize();
    void InvalidateCache(Position pos); // Метод для инвалидации кэша
    void UpdateDependencies(Position pos, const std::unordered_set<Position, PositionHash>& new_dependencies); // Обновление зависимостей
    bool HasCircularDependencies(Position pos, const std::unordered_set<Position, PositionHash>& dependencies); // Проверка на циклические зависимости
};

std::unique_ptr<SheetInterface> CreateSheet();