#include "cell.h"
#include "sheet.h" // Добавляем включение sheet.h

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

// Реализуйте следующие методы
Cell::Cell(Sheet& sheet) : impl_(std::make_unique<EmptyImpl>()), sheet_(sheet) {
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
    cache_valid_ = false;
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
    cache_valid_ = false;
}

Cell::Value Cell::GetValue() const {
    if (cache_valid_) {
        return cached_value_;
    }
    //auto cached_value = impl_->GetValue(sheet_);
    cached_value_ = impl_->GetValue(sheet_);
    cache_valid_ = true;
    return cached_value_;
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    //if (dynamic_cast<const FormulaImpl*>(impl_.get())) {
        return impl_->GetReferencedCells();
    //}
    //return {};
}

void Cell::InvalidateCache() {
    cache_valid_ = false;
}

bool Cell::IsReferenced() const {
    return !GetReferencedCells().empty();
}