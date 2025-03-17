#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:
//можете воспользоваться нашей подсказкой, но это необязательно.
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        // virtual void Set(std::string text) = 0;
        // virtual void Clear() = 0;
    };

    class EmptyImpl : public Impl {
    public:
        Value GetValue() const override {
            return ""; // Пустая ячейка возвращает пустую строку
        }
    
        std::string GetText() const override {
            return ""; // Пустая ячейка возвращает пустую строку
        }
    
        /* void Set(std::string text) override {
            // Изменяем реализацию в зависимости от содержимого
            if (text.empty()) {
                impl_ = std::make_unique<EmptyImpl>();
                // Остаемся пустой ячейкой
            } else if (text.front() == FORMULA_SIGN && text.size() > 1) {
                impl_ = std::make_unique<FormulaImpl>(text);
            } else {
                impl_ = std::make_unique<TextImpl>(text);
            }
        } */
    
        /* void Clear() override {
            // Ничего не делаем, так как ячейка уже пуста
        } */
    };

    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string text) : text_(std::move(text)) {
        }
    
        Value GetValue() const override {
            if (!text_.empty() && text_.front() == ESCAPE_SIGN) {
                return text_.substr(1); // Возвращаем текст без экранирующего символа
            }
            return text_;
        }
    
        std::string GetText() const override {
            return text_; // Возвращаем текст как есть
        }
    
        /* void Set(std::string text) override {
            text_ = std::move(text);
        } */
    
        /* void Clear() override {
            impl_ = std::make_unique<EmptyImpl>();
        } */
    
    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string text) {
            formula_ptr_ = ParseFormula(text.substr(1));
            formula_text_ = "=" + formula_ptr_->GetExpression();
        }
    
        Value GetValue() const override {
            FormulaInterface::Value value = formula_ptr_->Evaluate();
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            }
            return std::get<FormulaError>(value);
        }
    
        std::string GetText() const override {
            return formula_text_; // Возвращаем очищенную формулу
        }
    
        /* void Set(std::string text) override {
            formula_ptr_ = ParseFormula(std::move(text.substr(1)));
            formula_text_ = "=" + formula_ptr_->GetExpression();
        } */
    
        /* void Clear() override {
            impl_ = std::make_unique<EmptyImpl>();
        } */
    
    private:
        std::unique_ptr<FormulaInterface> formula_ptr_;
        std::string formula_text_;
    };

    std::unique_ptr<Impl> impl_;
};