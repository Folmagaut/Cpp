#include <cassert>
#include <cstddef>

template <typename T>
class UniquePtr {
private:
    T* ptr_;

public:
    UniquePtr() : ptr_(nullptr) {}

    explicit UniquePtr(T* ptr) : ptr_(ptr) {}

    // Запрет копирования
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Конструктор перемещения
    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    // Move-присваивание
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // Присваивание nullptr
    UniquePtr& operator=(nullptr_t) noexcept {
        delete ptr_;
        ptr_ = nullptr;
        return *this;
    }

    // Деструктор
    ~UniquePtr() {
        delete ptr_;
    }

    // Операторы разыменования
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }

    // Получение и сброс указателя
    T* Release() {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
    void Reset(T* ptr) {
        delete ptr_;
        ptr_ = ptr;
    }

    // Обмен содержимым
    void Swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
    }

    // Получение сырого указателя
    T* Get() const {
        return ptr_;
    }
};