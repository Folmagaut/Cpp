#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include "array_ptr.h"
#include <cstring>
#include <utility>

struct ReserveObj{
    ReserveObj(size_t size): size_(size){}
    size_t size_{};
};

ReserveObj Reserve(size_t size){
    return ReserveObj(size);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        capacity_ = size;
        size_ = size;
        if (size != 0){
            ArrayPtr<Type> tmp(size);
            items_.swap(tmp);
        }
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        capacity_ = size;
        size_ = size;
        if (size != 0){
            ArrayPtr<Type> tmp(size);
            items_.swap(tmp);
            auto tmp_it = items_.Get();
            while (tmp_it != end()){
                *tmp_it = value;
                ++tmp_it;
            }
        }
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        capacity_ = init.size();
        size_ = init.size();
        ArrayPtr<Type> tmp{init};
        items_.swap(tmp);
    }

    SimpleVector(const SimpleVector& other) {
        ArrayPtr<Type> tmp(other.capacity_);
        std::copy(other.begin(), other.end(), tmp.Get());
        items_.swap(tmp);
        capacity_ = other.capacity_;
        size_ = other.size_;
    }

    SimpleVector(SimpleVector&& other){
        ArrayPtr<Type> tmp(other.capacity_);
        for (size_t i = 0; i < other.GetSize(); ++i){
            tmp[i] = std::move(other.items_.Get()[i]);
        }
        items_.swap(tmp);
        other.capacity_ = std::exchange(capacity_, other.capacity_);
        other.size_ = std::exchange(size_, other.size_);
    }

    SimpleVector(const ReserveObj& res){
        this->Reserve(res.size_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return !size_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        SimpleVector<Type> tmp(rhs);
        this->swap(tmp);
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs){
        SimpleVector<Type> tmp(std::move(rhs));
        this->swap(tmp);
        return *this;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_){
            throw std::out_of_range("error");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_){
            throw std::out_of_range("error");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size < size_) {
            size_ = new_size;
            return;
        }
        if (new_size < capacity_){
            Type filling{};
            size_t old_size = size_;
            size_ = new_size;
            for (;old_size < size_;++old_size){
                items_[old_size] = std::move(filling);
            }
            return;
        }
        if (new_size > capacity_){
            ArrayPtr<Type> tmp(std::max(new_size, capacity_*2));
            std::move(std::make_move_iterator(items_.Get()),  std::make_move_iterator(end()), tmp.Get());
            items_.swap(tmp);
            capacity_ = std::max(new_size, capacity_*2);
            size_ = capacity_;
        }
    }

    void Reserve(size_t new_capacity){
        if (new_capacity > capacity_){
            ArrayPtr<Type> tmp(new_capacity);
            std::move(std::make_move_iterator(items_.Get()),  std::make_move_iterator(end()), tmp.Get());
            items_.swap(tmp);
            capacity_=new_capacity;
        } else{
            return;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        if (items_.Get() == nullptr){
            return items_.Get();
        }
        auto to_return = items_.Get();
        std::advance(to_return, size_);
        return to_return;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        if (items_.Get() == nullptr){
            return items_.Get();
        }
        auto to_return = items_.Get();
        std::advance(to_return, size_);
        return to_return;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        if (items_.Get() == nullptr){
            return items_.Get();
        }
        auto to_return = items_.Get();
        std::advance(to_return, size_);
        return to_return;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (size_ == capacity_){
            ResizeIfFull();
        }
        items_[size_] = item;
        ++size_;
    }

    void PushBack(Type&& item){
        if (size_ == capacity_){
            ResizeIfFull();
        }
        items_[size_] = std::move(item);
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        if (pos == end()){
            PushBack(value);
            return end()-1;
        }
        auto distance = pos - begin();
        if (size_ < capacity_){
            std::copy_backward(items_.Get() + distance, end(), end()+1);
            items_[distance] = value;
            ++size_;
        } else if (size_ == capacity_){
            ResizeIfFull();
            std::copy_backward(items_.Get() + distance, end(), end()+1);
            items_[distance] = value;
            ++size_;
        }
        return begin()+distance;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        if (pos == end()){
            PushBack(std::move(value));
            return end()-1;
        }
        auto distance = pos - begin();
        if (size_ < capacity_){
            std::move_backward(std::make_move_iterator(items_.Get() + distance), std::make_move_iterator(end()), std::make_move_iterator(end()+1));
            items_[distance] = std::move(value);
            ++size_;
        } else if (size_ == capacity_){
            ResizeIfFull();
            std::move_backward(std::make_move_iterator(items_.Get() + distance), std::make_move_iterator(end()), std::make_move_iterator(end()+1));
            items_[distance] = std::move(value);
            ++size_;
        }
        return begin()+distance;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        auto distance = pos - items_.Get();
        std::move_backward(std::make_move_iterator(items_.Get() + distance+1), std::make_move_iterator(end()), std::make_move_iterator(end()));
        --size_;
        return begin()+distance+1;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        items_.swap(other.items_);
    }

private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void FillArray(Type value){
        auto tmp_it = items_.Get();
            while (tmp_it != end()){
                if (*tmp_it == Type()){
                    *tmp_it = value;
                    ++tmp_it;
                }
            }
    }
    
    void ResizeIfFull(){
        if (capacity_ == 0){
            ArrayPtr<Type> tmp(1);
            capacity_ = 1;
            items_.swap(tmp);
            return;
        }
        ArrayPtr<Type> tmp(capacity_*2);
        std::move(std::make_move_iterator(items_.Get()),  std::make_move_iterator(end()), tmp.Get());
        items_.swap(tmp);
        capacity_*=2;
        return;
    }

};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() == rhs.GetSize()){
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
    return false;
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
} 