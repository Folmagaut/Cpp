#pragma once

#include <array>
#include <stdexcept>

template <typename T, size_t N>
class StackVector {
public:
    using iterator = typename std::array<T, N>::iterator;
    using const_iterator = typename std::array<T, N>::const_iterator;

    //explicit StackVector() : vec_size_(0) {
    //}

    //explicit StackVector(size_t a_size = 0) : vec_size_(a_size > N ? N : a_size) {
    //}

    explicit StackVector(size_t a_size = 0) : vec_size_(std::min(a_size, N)) {
        if (a_size > N) {
            throw std::invalid_argument("Size exceeds capacity");
        }
    }
    T& operator[](size_t index) {
        if (index >= vec_size_) {
            throw std::out_of_range("Index out of range");
        }
        return stack_vec_[index];
    }
    const T& operator[](size_t index) const {
        if (index >= vec_size_) {
            throw std::out_of_range("Index out of range");
        }
        return stack_vec_[index];
    }

    iterator begin() {
        return stack_vec_.begin();
    }
    const_iterator begin() const {
        return stack_vec_.begin();
    }
    iterator end() {
        return stack_vec_.begin() + vec_size_;
    }
    const_iterator end() const {
        return stack_vec_.begin() + vec_size_;
    }

    size_t Size() const {
        return vec_size_;
    }
    size_t Capacity() const {
        return N;
    }

    void PushBack(const T& value) {
        if (vec_size_ == N) {
            throw std::overflow_error("Vector is full");
        }
        stack_vec_[vec_size_++] = value;
    }
    T PopBack() {
        if (vec_size_ == 0) {
            throw std::underflow_error("Vector is empty");
        }
        return stack_vec_[--vec_size_];
    }

private:
    std::array<T, N> stack_vec_;
    size_t vec_size_;
};
/*
// авторское
#pragma once

#include <array>
#include <stdexcept>

template <typename T, size_t N>
class StackVector {
public:
    explicit StackVector(size_t a_size = 0)
            : size(a_size) {
        if (size > N) {
            using namespace std::literals;
            throw std::invalid_argument("Too much"s);
        }
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    auto begin() {
        return data.begin();
    }

    auto end() {
        return data.begin() + size;
    }

    auto begin() const {
        return data.begin();
    }

    auto end() const {
        return data.begin() + size;
    }

    size_t Size() const {
        return size;
    }

    size_t Capacity() const {
        return N;
    }

    void PushBack(const T& value) {
        if (size >= N) {
            using namespace std::literals;
            throw std::overflow_error("Full"s);
        } else {
            data[size++] = value;
        }
    }

    T PopBack() {
        if (size == 0) {
            using namespace std::literals;
            throw std::underflow_error("Empty"s);
        } else {
            return data[--size];
        }
    }

private:
    std::array<T, N> data;
    size_t size = 0;
};
*/