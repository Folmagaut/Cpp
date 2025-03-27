#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h" // Добавляем включение sheet.h

#include <list>
#include <unordered_set>

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet); // Добавляем ссылку на Sheet
    ~Cell();

    void Set(std::string text) override;
    void Clear() override;

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override; // Реализация метода

private:
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual Value GetValue(const SheetInterface& sheet) const = 0; // Добавляем параметр SheetInterface
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const = 0; // Новый метод
    };

    class EmptyImpl : public Impl {
    public:
        Value GetValue(const SheetInterface& sheet) const override {
            return "";
        }

        std::string GetText() const override {
            return "";
        }
        std::vector<Position> GetReferencedCells() const override {
            return {};
        }
    };

    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string text) : text_(std::move(text)) {}

        Value GetValue(const SheetInterface& sheet) const override {
            if (!text_.empty() && text_.front() == ESCAPE_SIGN) {
                return text_.substr(1);
            }
            return text_;
        }

        std::string GetText() const override {
            return text_;
        }
        std::vector<Position> GetReferencedCells() const override {
            return {};
        }

    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string text);

        Value GetValue(const SheetInterface& sheet) const override;
        std::string GetText() const override;
        std::vector<Position> GetReferencedCells() const override;

    private:
        std::unique_ptr<FormulaInterface> formula_ptr_;
        std::string formula_text_;
    };

    struct PositionHash {
        size_t operator()(const Position& pos) const {
            return std::hash<std::string>()(pos.ToString());
        }
    };

    std::unique_ptr<Impl> impl_;
    Sheet& sheet_; // Добавляем ссылку на Sheet
    std::unordered_set<Position, PositionHash> referenced_cells_; // Добавляем множество зависимостей
    Value cached_value_; // Добавляем кэш
    bool cache_valid_ = false; // Добавляем флаг валидности кэша
};