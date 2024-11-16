#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>

#include "bimap.h"

struct BiMap::Impl {
//private:
    std::unordered_map<std::string, std::string> key_to_value;
    std::unordered_map<std::string, std::string> value_to_key;
};

// Конструктор по умолчанию
BiMap::BiMap()
    : impl_(std::make_unique<Impl>()) {
}

// Деструктор
BiMap::~BiMap() = default;

// Конструктор копирования 
BiMap::BiMap(const BiMap& other)
    : impl_(other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr) {
}

// и перемещения
BiMap::BiMap(BiMap&& other) noexcept = default;
/*     : impl_(std::move(other.impl_)) {
    other.impl_.reset(nullptr);
} */

// Операция копирования
BiMap& BiMap::operator=(const BiMap& other) {
    if (this != std::addressof(other)) {
        impl_ = other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr;
    }
    /* if (this != &other) {
        if (!other.impl_) {
            impl_.reset(nullptr);
        } else if (impl_) {
            assert(other.impl_);
            *impl_ = *other.impl_;
        } else {
            assert(!impl_ && other.impl_);
            impl_ = std::make_unique<Impl>(*other.impl_);
        }
    } */
    return *this;
}

// и переноса
BiMap& BiMap::operator=(BiMap&& other) noexcept = default; /* {
    impl_ = std::move(other.impl_);
    other.impl_.reset(nullptr);
    return *this;
} */


bool BiMap::Add(std::string_view key, std::string_view value) {

    if (!impl_->key_to_value.count(std::string(key)) && !impl_->value_to_key.count(std::string(value))) {
        impl_->key_to_value[std::string(key)] = std::string(value);
        impl_->value_to_key[std::string(value)] = std::string(key);
        return true;
    } else {
        return false;
    }
}

std::optional<std::string_view> BiMap::FindValue(std::string_view key) const noexcept {
    const auto it = impl_->key_to_value.find(std::string(key));
    if (it != impl_->key_to_value.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

std::optional<std::string_view> BiMap::FindKey(std::string_view value) const noexcept {
    const auto it = impl_->value_to_key.find(std::string(value));
    if (it != impl_->value_to_key.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

// авторское
/*
#include "bimap.h"

#include <cassert>
#include <deque>
#include <unordered_map>
#include <string>

struct BiMap::Impl {
    Impl() = default;

    Impl(const Impl& other)
        : items_(other.items_) {
        for (auto& [k, v] : items_) {
            key_to_value_[k] = v;
            value_to_key_[v] = k;
        }
    }

    // Присваивание реализовано в BiMap за счёт создания нового объекта Impl
    Impl& operator=(const Impl& other) = delete;

    bool Add(std::string_view key, std::string_view value) {
        if (FindValue(key) || FindKey(value)) {
            return false;
        }

        items_.push_back(Item{std::string(key), std::string(value)});

        const auto& inserted_item = items_.back();
        std::string_view k = inserted_item.key;
        std::string_view v = inserted_item.value;

        try {
            key_to_value_.emplace(k, v);
            value_to_key_.emplace(v, k);
        } catch (...) {
            // Откатываем изменения в случае выброшенного исключения
            key_to_value_.erase(k);
            value_to_key_.erase(v);
            items_.pop_back();
            throw;
        }

        return true;
    }

    std::optional<std::string_view> FindValue(std::string_view key) const noexcept {
        if (auto pos = key_to_value_.find(key); pos != key_to_value_.end()) {
            return pos->second;
        }
        return {};
    }

    std::optional<std::string_view> FindKey(std::string_view value) const noexcept {
        if (auto pos = value_to_key_.find(value); pos != value_to_key_.end()) {
            return pos->second;
        }
        return {};
    }

private:
    struct Item {
        std::string key;
        std::string value;
    };

    std::deque<Item> items_;
    std::unordered_map<std::string_view, std::string_view> key_to_value_;
    std::unordered_map<std::string_view, std::string_view> value_to_key_;
};

BiMap::BiMap()
    : impl_(std::make_unique<Impl>()) {
}

BiMap::BiMap(BiMap&& other) noexcept = default;

BiMap& BiMap::operator=(BiMap&& other) noexcept = default;

BiMap::BiMap(const BiMap& other)
    : impl_{other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr} {
}

BiMap& BiMap::operator=(const BiMap& other) {
    if (this != std::addressof(other)) {
        impl_ = other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr;
    }
    return *this;
}

BiMap::~BiMap() = default;

bool BiMap::Add(std::string_view key, std::string_view value) {
    return impl_->Add(key, value);
}

std::optional<std::string_view> BiMap::FindValue(std::string_view key) const noexcept {
    return impl_->FindValue(key);
}

std::optional<std::string_view> BiMap::FindKey(std::string_view value) const noexcept {
    return impl_->FindKey(value);
}
*/