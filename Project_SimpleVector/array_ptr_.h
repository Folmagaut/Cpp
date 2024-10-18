#pragma once 
 
#include <algorithm> 
#include <cassert> 
#include <cstdlib> 
#include <utility> 
 
template <typename Type> 
class ArrayPtr { 
public: 
    ArrayPtr () = default; 
 
    explicit ArrayPtr (std::size_t size) 
        : raw_ptr_(size == 0 ? nullptr :  new Type[size]{}) {} 
  
    explicit ArrayPtr (Type* raw_ptr) noexcept 
        : raw_ptr_(raw_ptr) {} 
 
    ArrayPtr (const ArrayPtr&) = delete; 
 
    ArrayPtr (ArrayPtr&& other) noexcept 
        : raw_ptr_(other.raw_ptr_) { 
        other.raw_ptr_ = nullptr; 
    } 
 
    ~ArrayPtr () { 
        delete[] raw_ptr_; 
    } 
 
    ArrayPtr& operator= (ArrayPtr&& other) noexcept { 
        if (this != &other) { 
            swap(other); 
        } 
        return *this; 
    } 
 
    ArrayPtr& operator= (const ArrayPtr&) = delete; 
 
    Type* Release() noexcept { 
        Type* raw_ptr = raw_ptr_; 
        raw_ptr_ = nullptr; 
        return raw_ptr; 
    } 
 
    Type& operator[] (std::size_t index) noexcept { 
        return raw_ptr_[index]; 
    } 
 
    const Type& operator[] (std::size_t index) const noexcept { 
        return raw_ptr_[index]; 
    } 
 
    explicit operator bool() const { 
        return raw_ptr_ != nullptr; 
    } 
 
    Type* Get() const noexcept { 
        return raw_ptr_; 
    } 
 
    void swap(ArrayPtr& other) noexcept { 
        Type* x = raw_ptr_; 
        raw_ptr_ = other.raw_ptr_; 
        other.raw_ptr_ = x; 
    } 
 
private: 
    Type* raw_ptr_ = nullptr; 
};
