#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override; // Реализация метода

    bool IsReferenced() const;

    void InvalidateCache(); // Добавляем публичный метод

private:
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual Value GetValue(const SheetInterface& sheet) const = 0; // Добавляем параметр
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const {
            return {};
        } // Новый метод
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
        explicit FormulaImpl(std::string text) : formula_(ParseFormula(text.substr(1))) {}

        Value GetValue(const SheetInterface& sheet) const override {
            FormulaInterface::Value value = formula_->Evaluate(sheet);
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            }
            return std::get<FormulaError>(value);
        }

        std::string GetText() const override {
            return "=" + formula_->GetExpression();
        }

        std::vector<Position> GetReferencedCells() const override {
            return formula_->GetReferencedCells();
        }
    private:
        std::unique_ptr<FormulaInterface> formula_;
    };

    std::unique_ptr<Impl> impl_;

    // Добавьте поля и методы для связи с таблицей, проверки циклических 
    // зависимостей, графа зависимостей и т. д.
    std::unordered_set<Position, PositionHash> referenced_cells_; // Добавляем множество зависимостей

    Sheet& sheet_;
    mutable bool cache_valid_ = false;
    mutable Value cached_value_;
};