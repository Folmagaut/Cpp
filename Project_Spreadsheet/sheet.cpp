#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::ResizeCells(Position pos) {
    if (cells_.size() <= static_cast<size_t>(pos.row)) {
        cells_.resize(static_cast<size_t>(pos.row) + 1);
    }
    if (cells_[pos.row].size() <= static_cast<size_t>(pos.col)) {
        cells_[pos.row].resize(static_cast<size_t>(pos.col) + 1);
    }
}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    ResizeCells(pos);
    cells_[pos.row][pos.col] = std::make_unique<Cell>();
    cells_[pos.row][pos.col]->Set(text);
    UpdatePrintableSize(pos);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    if (static_cast<size_t>(pos.row) < cells_.size() && static_cast<size_t>(pos.col) < cells_[pos.row].size() && cells_[pos.row][pos.col]) {
        return cells_[pos.row][pos.col].get();
    }
    return nullptr;
}
CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    if (static_cast<size_t>(pos.row) < cells_.size() && static_cast<size_t>(pos.col) < cells_[pos.row].size() && cells_[pos.row][pos.col]) {
        return cells_[pos.row][pos.col].get();
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    if (static_cast<size_t>(pos.row) < cells_.size() && static_cast<size_t>(pos.col) < cells_[pos.row].size()) {
        cells_[pos.row][pos.col].reset(); //->Clear();
        RecalculatePrintableSize(); // Обновляем размер
    }
}

Size Sheet::GetPrintableSize() const {
    return printable_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    for (int row = 0; row < printable_size_.rows; ++row) {
        for (int col = 0; col < printable_size_.cols; ++col) {
            Position pos = {row, col};
            const CellInterface* cell = GetCell(pos);
            if (cell) {
                std::visit([&output](const auto& value) {
                    output << value;
                }, cell->GetValue());
            }
            if (col < printable_size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    for (int row = 0; row < printable_size_.rows; ++row) {
        for (int col = 0; col < printable_size_.cols; ++col) {
            Position pos = {row, col};
            const CellInterface* cell = GetCell(pos);
            if (cell) {
                output << cell->GetText();
            }
            if (col < printable_size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}

void Sheet::UpdatePrintableSize(Position pos) {
    printable_size_.rows = std::max(printable_size_.rows, pos.row + 1);
    printable_size_.cols = std::max(printable_size_.cols, pos.col + 1);
}

void Sheet::RecalculatePrintableSize() {
    printable_size_ = {0, 0};
    for (size_t row = 0; row < cells_.size(); ++row) {
        for (size_t col = 0; col < cells_[row].size(); ++col) {
            if (cells_[row][col]) {
                printable_size_.rows = std::max(printable_size_.rows, static_cast<int>(row) + 1);
                printable_size_.cols = std::max(printable_size_.cols, static_cast<int>(col) + 1);
            }
        }
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}