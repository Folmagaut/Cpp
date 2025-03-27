#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

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

	// Можете дополнить ваш класс нужными полями и методами
    void UpdateDependencies(Position pos, const std::vector<Position>& new_dependencies);
    void InvalidateCache(Position pos);
    bool HasCircularDependencies(Position pos);

private:
	// Можете дополнить ваш класс нужными полями и методами
    std::vector<std::vector<std::unique_ptr<Cell>>> cells_;
    Size printable_size_ = {0, 0};

    std::unordered_map<Position, std::unordered_set<Position, PositionHash>, PositionHash> dependencies_;

    void ResizeCells(Position pos);
    void UpdatePrintableSize(Position pos);
    void RecalculatePrintableSize();
};

std::unique_ptr<SheetInterface> CreateSheet();