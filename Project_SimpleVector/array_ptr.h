#pragma once

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

// Умный указатель, удаляющий связанный объект при своём разрушении.
// Параметр шаблона T задаёт тип объекта, на который ссылается указатель
template <typename Type>
class ScopedPtr {
public:
    // Конструктор по умолчанию создаёт нулевой указатель,
    // так как поле ptr_ имеет значение по умолчанию nullptr
    ScopedPtr() = default;

    // Создаёт указатель, ссылающийся на переданный raw_ptr.
    // raw_ptr ссылается либо на объект, созданный в куче при помощи new,
    // либо является нулевым указателем
    // Спецификатор noexcept обозначает, что метод не бросает исключений
    explicit ScopedPtr(Type* raw_ptr) noexcept : ptr_(raw_ptr) {
    }

    // указатель на новый массив
    explicit ScopedPtr(size_t size) {
        if (size == 0) {
            ptr_ = nullptr;
        } else {
            ptr_ = new Type[size];
        }
    }

    // Удаляем у класса конструктор копирования
    ScopedPtr(const ScopedPtr&) = delete;

    // Деструктор. Удаляет объект, на который ссылается умный указатель.
    ~ScopedPtr() {
        delete[] ptr_;
    }

    // Возвращает указатель, хранящийся внутри ScopedPtr
    Type* GetRawPtr() const noexcept {
        return ptr_;
    }

    // Прекращает владение объектом, на который ссылается умный указатель.
    // Возвращает прежнее значение "сырого" указателя и устанавливает поле ptr_ в null
    Type* Release() noexcept {
        Type* ret_ptr = ptr_;
        ptr_ = nullptr;
        return ret_ptr;
    }

    // Оператор приведения к типу bool позволяет узнать, ссылается ли умный указатель
    // на какой-либо объект
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    // Оператор разыменования возвращает ссылку на объект
    // Выбрасывает исключение std::logic_error, если указатель нулевой
    Type& operator*() const {
        if (!ptr_) {
            throw logic_error("Error: ptr_ is null"s);
        }
        return *ptr_;
    }

    // Оператор -> должен возвращать указатель на объект
    // Выбрасывает исключение std::logic_error, если указатель нулевой
    Type* operator->() const {
        if (!ptr_) {
            throw logic_error("Error: ptr_ is null"s);
        }
        return ptr_;
    }

    Type& operator[](size_t index) noexcept {
        return ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return ptr_[index];
    }

    void swap(ScopedPtr& other) noexcept {
        std::swap(other.ptr_, ptr_);
    }

private:
    Type* ptr_ = nullptr;
};