#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

// Реализуйте следующие методы
Cell::Cell() : impl_(std::make_unique<EmptyImpl>()) {
}

Cell::~Cell() {
}

void Cell::Set(std::string text) {
    // Изменяем реализацию в зависимости от содержимого
    if (text.empty()) {
        // Будем пустой ячейкой
        impl_ = std::make_unique<EmptyImpl>();
    } else if (text.front() == FORMULA_SIGN && text.size() > 1) {
        impl_ = std::make_unique<FormulaImpl>(text);
    } else {
        impl_ = std::make_unique<TextImpl>(text);
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}