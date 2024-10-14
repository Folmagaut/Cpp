#pragma once

#include "array_ptr.h"
#include <algorithm>
#include <cassert>
#include <initializer_list>

#include <stdexcept>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity) : capacity_(capacity) {
    }

    size_t GetCapacity() {
        return capacity_;
    }

private:
    size_t capacity_;
};

template <typename Type>
class SimpleVector {
public:
    //#define DEFAULT_VALUE = Type();
    using Iterator = Type*;
    using ConstIterator = const Type*;

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            size_t prev_size = size_;
            SimpleVector tmp(new_capacity);
            std::copy(begin(), begin() + capacity_, tmp.simp_vec_start_ptr_.GetRawPtr());
            swap(tmp);
            capacity_ = new_capacity;
            size_ = prev_size;
        }
    }

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : SimpleVector(size, Type()) {
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : simp_vec_start_ptr_(new Type[size]), size_(size), capacity_(size) {
        std::fill(simp_vec_start_ptr_.GetRawPtr(), simp_vec_start_ptr_.GetRawPtr() + size_, value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : simp_vec_start_ptr_(new Type[init.size()]), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), simp_vec_start_ptr_.GetRawPtr());
    }

    // конструктор копирования
    SimpleVector(const SimpleVector& other) : simp_vec_start_ptr_(other.capacity_), size_(other.size_), capacity_(other.capacity_) {
        //simp_vec_start_ptr_.Release();
        std::copy(other.begin(), other.end(), simp_vec_start_ptr_.GetRawPtr());
    }

    // конструктор присваивания
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }
    // конструктор через reserve
    SimpleVector(ReserveProxyObj obj) {
        Reserve(obj.GetCapacity());
    }

    // деструктор
    ~SimpleVector() {
        delete[] simp_vec_start_ptr_.Release();
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
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return simp_vec_start_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return simp_vec_start_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }
        return simp_vec_start_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }
        return simp_vec_start_ptr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        } else if (new_size <= capacity_) {
            std::fill(simp_vec_start_ptr_.GetRawPtr() + size_, simp_vec_start_ptr_.GetRawPtr() + new_size, Type());
            size_ = new_size;
        }
        if (new_size > capacity_) {
            size_t new_capacity = std::max(new_size, capacity_ * 2);
            SimpleVector tmp(new_capacity);
            std::copy(begin(), begin() + capacity_, tmp.simp_vec_start_ptr_.GetRawPtr());
            swap(tmp);
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return simp_vec_start_ptr_.GetRawPtr();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return simp_vec_start_ptr_.GetRawPtr() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return simp_vec_start_ptr_.GetRawPtr();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return simp_vec_start_ptr_.GetRawPtr() + size_;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {        
        size_t prev_size = size_;
        if (size_ + 1 > capacity_) {
            size_t n = 1;
            Resize(std::max(n, capacity_ * 2));
            size_ = prev_size;
        }
        simp_vec_start_ptr_[size_++] = item;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        if (pos < begin() || pos > end()) {
            throw std::out_of_range("Iterator out of range");
        }
        size_t index = pos - cbegin();
        size_t prev_size = size_;
        if (size_ + 1 > capacity_) {
            size_t n = 1;
            Resize(std::max(n, capacity_ * 2));
            size_ = prev_size;
        }
        Iterator nonconst_pos = begin() + index;
        std::copy_backward(nonconst_pos, end(), end() + 1);
        *nonconst_pos = value;
        ++size_;
        return nonconst_pos;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size_ > 0) {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        if (pos < begin() || pos >= end()) {
            throw std::out_of_range("Iterator out of range");
        }
        size_t index = pos - cbegin();
        Iterator nonconst_pos = begin() + index;
        std::copy_backward(nonconst_pos + 1, end(), end() - 1);
        --size_;
        return nonconst_pos;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        simp_vec_start_ptr_.swap(other.simp_vec_start_ptr_);
    }

private:
    ScopedPtr<Type> simp_vec_start_ptr_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    if (&lhs == &rhs) {
        return true;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}