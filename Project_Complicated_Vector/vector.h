#pragma once
#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>
#include <utility>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory& rhs) = delete;
    
    RawMemory(RawMemory&& other) noexcept { 
        buffer_ = other.buffer_;
        capacity_ = other.capacity_;
        other.buffer_ = nullptr;
        other.capacity_ = 0;
    }

    RawMemory& operator=(RawMemory&& rhs) noexcept {
        if (this != &rhs) {
            //buffer_->~RawMemory();
            //capacity_ = 0;
            Swap(rhs);
        }
        return *this;
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector {
public:
    using iterator = T*;
    using const_iterator = const T*;

    Vector() = default;

    explicit Vector(size_t size)
    : data_(size)
    , size_(size)  //
    {
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }

    Vector(const Vector& other)
        : data_(other.size_)
        , size_(other.size_)  
    {
        std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());
    }

    Vector(Vector&& other) noexcept {
        Swap(other);
    }

    Vector& operator=(Vector&& rhs) noexcept {
        if (this != &rhs) {
            Swap(rhs);
        }
        return *this;
    }

    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            if (rhs.size_ > data_.Capacity()) {
                // copy-and-swap
                Vector rhs_copy(rhs);
                Swap(rhs_copy);
            }
            else {
                if (rhs.size_ < size_) {
                    std::copy(rhs.data_.GetAddress(), rhs.data_.GetAddress() + rhs.size_, data_.GetAddress());
                    std::destroy_n(data_.GetAddress() + rhs.size_, size_ - rhs.size_);
                } else {
                    std::copy(rhs.data_.GetAddress(), rhs.data_.GetAddress() + size_, data_.GetAddress());
                    std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, rhs.size_ - size_, data_.GetAddress() + size_);
                }
                size_ = rhs.size_;
            }
        }
        return *this;
    }

    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }
    //В константном операторе [] используется оператор  const_cast,
    //чтобы снять константность с ссылки на текущий объект и вызвать неконстантную версию оператора [].
    //Так получится избавиться от дублирования проверки assert(index < size).
    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= data_.Capacity()) {
            return;
        }
        RawMemory<T> new_data(new_capacity);
        // constexpr оператор if будет вычислен во время компиляции
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
        } else {
            std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
        }
        // Разрушаем элементы в data_
        std::destroy_n(data_.GetAddress(), size_);
        // Избавляемся от старой сырой памяти, обменивая её на новую
        data_.Swap(new_data);
        // При выходе из метода старая память будет возвращена в кучу
    }

    void Resize(size_t new_size) {
        if (new_size > size_) {
            Reserve(new_size);
            std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
        } else {
            std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);
        }
        size_ = new_size;
    }

    void PushBack(const T& value) {
        /* if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
                new (new_data + size_) T(value);
                if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                    std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
                }
                else {
                    std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
                }
                std::destroy_n(data_.GetAddress(), size_);
                data_.Swap(new_data);
            }
            else {
                new (data_ + size_) T(value);
            }
            ++size_; */
        EmplaceBack(value);
    }

    void PushBack(T&& value) {
        /* if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
                new (new_data + size_) T(std::forward<T>(value));
                if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                    std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
                }
                else {
                    std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
                }
                std::destroy_n(data_.GetAddress(), size_);
                data_.Swap(new_data);
            }
            else {
                new (data_ + size_) T(std::forward<T>(value));
            }
            ++size_; */
            EmplaceBack(std::forward<T>(value));
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
       if (size_ == Capacity()) {
        RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            new (new_data + size_) T(std::forward<Args>(args)...);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            } else {
                try {
                    std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
                }
                catch (...) {
                    std::destroy_n(new_data.GetAddress() + size_, 1);
                    throw;
                }  
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
        } else {
            new (data_ + size_) T(std::forward<Args>(args)...);
        }
        ++size_;
        return data_[size_ - 1];
    }

    void PopBack() noexcept {
        if (size_ != 0) {
            std::destroy_at(data_.GetAddress() + size_ - 1);
            --size_;
        }
    }

    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args) {
        if (pos < begin() || pos > end()) {
            throw std::out_of_range("Итератор находится вне диапазона");
        }

        size_t index = pos - cbegin();
        iterator nonconst_pos = begin() + index;

        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            new (new_data + index) T(std::forward<Args>(args)...);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(begin(), index, new_data.GetAddress());
                std::uninitialized_move_n(nonconst_pos, size_ - index, new_data.GetAddress() + index + 1);
            } else {
                try {
                    std::uninitialized_copy_n(begin(), index, new_data.GetAddress());
                    std::uninitialized_copy_n(nonconst_pos, size_ - index, new_data.GetAddress() + index + 1);
                }
                catch (...) {
                    std::destroy_n(new_data.GetAddress() + index, 1);
                    throw;
                }
            }
            std::destroy_n(begin(), size_);
            data_.Swap(new_data);
            nonconst_pos = begin() + index;
        } else if (size_ > 0 && index < size_) {
            auto new_obj = T(std::forward<Args>(args)...);
            new (end()) T(std::move(*(end() - 1))); // передвигаем последний элемент вправо, если есть, что передвигать
            try {
                std::move_backward(nonconst_pos, end() - 1, end());
            }
            catch (...) {
                std::destroy_n(end(), 1);
                throw;
            }
            data_[index] = std::move(new_obj);
        } else {
            new (nonconst_pos) T(std::forward<Args>(args)...);
        }
        /* if (size_ == index) {
            new (nonconst_pos) T(std::forward<Args>(args)...);
        } else {
            auto new_obj = T(std::forward<Args>(args)...);
            new (data_.GetAddress() + size_) T(std::move(*(end()-1)));
            try {
                std::move_backward(nonconst_pos, end() - 1 , end());
            }
            catch (...) {
                std::destroy_n(end(), 1);
                throw;
            }
            data_[index] = std::move(new_obj);
        } */
        

            

        ++size_;
        return nonconst_pos;
    }

    iterator Insert(const_iterator pos, const T& value) {
        return Emplace(pos, value);
    }

    iterator Insert(const_iterator pos, T&& value) {
        return Emplace(pos, std::forward<T>(value));
    }

    iterator Erase(const_iterator pos) {
        if (pos < begin() || pos > end()) {
            throw std::out_of_range("Итератор находится вне диапазона");
        }
        size_t index = pos - begin();
        iterator nonconst_pos = begin() + index;
        std::move(nonconst_pos + 1, end(), nonconst_pos);
        PopBack();
        return nonconst_pos;
    }

    
    void Swap(Vector& other) noexcept {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
    }

    iterator begin() noexcept {
        return data_.GetAddress();
    }
    iterator end() noexcept {
        return data_.GetAddress() + size_;
    }
    const_iterator begin() const noexcept {
        return cbegin();
    }
    const_iterator end() const noexcept {
        return cend();
    }
    const_iterator cbegin() const noexcept {
        return data_.GetAddress();
    }
    const_iterator cend() const noexcept {
        return data_.GetAddress() + size_;
    }
private:
    RawMemory<T> data_;
    size_t size_ = 0;
};