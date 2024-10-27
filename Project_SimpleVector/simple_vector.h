#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>
#include "array_ptr.h"

using std::literals::string_literals::operator ""s;

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity) : capacity_(capacity) {
    }

    [[nodiscard]] size_t GetCapacity() const {
        return capacity_;
    }
private:
    size_t capacity_;
};

template <typename Type>
class SimpleVector {
public:
    using Default_value = Type;
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)              // не понял, это отлично или надо что-то поправить? :)
        : SimpleVector(size, Default_value()) {
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        : simp_vec_start_ptr_(size),                // исправил
        size_(size),                                // размер и вместимость иницилизированы через список инициализации,
        capacity_(size) {                           // как и полагается
        std::fill_n(simp_vec_start_ptr_.GetRawPtr(), size, value); // почитать
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : simp_vec_start_ptr_(init.size()),         // исправил
        size_(init.size()),
        capacity_(init.size()) {
        std::copy(init.begin(), init.end(), simp_vec_start_ptr_.GetRawPtr());
    }

    // конструктор копирования
    SimpleVector(const SimpleVector& other)
        : simp_vec_start_ptr_(other.capacity_),     // тут всё нормально
        size_(other.size_),
        capacity_(other.capacity_) {
        std::copy(other.begin(), other.end(), simp_vec_start_ptr_.GetRawPtr());
    }
    
    // конструктор через reserve
    SimpleVector(ReserveProxyObj obj)
        : simp_vec_start_ptr_(obj.GetCapacity()),   // тут тоже
        size_(0),
        capacity_(obj.GetCapacity()) {
    }

    // конструктор перемещения для move
    SimpleVector(SimpleVector&& other)
        : simp_vec_start_ptr_(std::move(other.simp_vec_start_ptr_)), // этот конструктор в array_ptr есть
        size_(std::exchange(other.size_, 0)),
        capacity_(std::exchange(other.capacity_, 0)) {
    }

    // конструктор присваивания
    SimpleVector& operator=(const SimpleVector& rhs) { // тут, вроде, нормально
        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // конструктор присваивания для move
    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            simp_vec_start_ptr_.swap(rhs.simp_vec_start_ptr_); // нормально
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
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
            throw std::out_of_range("Индекс находится вне диапазона"s);
        }
        return simp_vec_start_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Индекс находится вне диапазона"s);
        }
        return simp_vec_start_ptr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // резервирует место
    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        ScopedPtr<Type> tmp(new_capacity);
        std::move(begin(), begin() + capacity_, tmp.GetRawPtr());
        simp_vec_start_ptr_.swap(tmp);
        capacity_ = new_capacity;
    }
    
    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= capacity_) {
            for (size_t index = size_; index < new_size; ++index) {
                simp_vec_start_ptr_[index] = Default_value();
            }
        } else {
            size_t new_capacity = std::max(new_size, capacity_ * 2);
            ScopedPtr<Type> tmp(new_capacity);

            if (size_ != 0) {
                std::move(begin(), end(), tmp.GetRawPtr());
            }
            for (size_t index = size_; index < new_size; ++index) {
                tmp[index] = Default_value();
            }
            simp_vec_start_ptr_.swap(tmp);
            capacity_ = new_capacity;
        }
        size_ = new_size;
    }
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {        
        if (size_ + 1 > capacity_) {
            Reserve(std::max(size_ + 1, capacity_ * 2));
        }
        simp_vec_start_ptr_[size_++] = item;
    }

    void PushBack(Type&& item) {        
        if (size_ + 1 > capacity_) {
            Reserve(std::max(size_ + 1, capacity_ * 2));
        }
        simp_vec_start_ptr_[size_++] = std::move(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        if (pos < begin() || pos > end()) {
            throw std::out_of_range("Итератор находится вне диапазона"s);
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

    // для перемещаемых объектов
    Iterator Insert(ConstIterator pos, Type&& value) {
        if (pos < begin() || pos > end()) {
            throw std::out_of_range("Итератор находится вне диапазона"s);
        }
        size_t index = pos - cbegin();
        size_t prev_size = size_;
        if (size_ + 1 > capacity_) {
            Resize(std::max(size_ + 1, capacity_ * 2));
            size_ = prev_size;
        }
        Iterator nonconst_pos = begin() + index;
        std::move_backward(nonconst_pos, end(), end() + 1);
        simp_vec_start_ptr_[index] = std::move(value);
        ++size_;
        return nonconst_pos;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert (size_ > 0);             // исправил
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        if (pos < begin() || pos >= end()) {
            throw std::out_of_range("Итератор находится вне диапазона"s);
        }
        size_t index = pos - cbegin();
        Iterator nonconst_pos = (begin() + index);
        std::move(nonconst_pos + 1, end(), nonconst_pos);
        --size_;
        return nonconst_pos;
    }

    // самый полезный метод :)
    void swap(SimpleVector& other) noexcept {
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        simp_vec_start_ptr_.swap(other.simp_vec_start_ptr_);
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
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
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