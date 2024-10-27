
// Online C++ Compiler - Build, Compile and Run your C++ programs online in your favorite browser

#include<iostream>
#include<vector>

using namespace std;

struct Number {
public:
    int& getNumberRef() { // This is lvalue!
        return x;
    }
    
    int getNumber() { // This is rvalue!
        return x;
    }
    int x = 0;
};

int main()
{
    Number num;
    num.getNumberRef() = 42;
    // num.getNumber() = 42; // compile error
    cout << num.x << endl;
    return 0;
}

/* 
SimpleVector& operator=(SimpleVector&& rhs) noexcept {

        if (this != &rhs) { 

            data_ = std::move(rhs.data_);

            size_ = rhs.size_;

            capacity_ = rhs.capacity_;

 

 

            rhs.size_ = 0;

            rhs.capacity_ = 0;

        }

        return *this;

    } */

/* #include <string>
#include <iostream>

class Spy {
public:
    Spy(int i_) : i(i_) {
        std::cout << "Spy(i)" << std::endl;
    }

    Spy(Spy&& other) : i(other.i) {
        std::cout << "Spy(Spy&&)" << std::endl;
    }

    Spy(const Spy& other) : i(other.i) {
        std::cout << "Spy(const Spy&)" << std::endl;
    }

    ~Spy() {
        std::cout << "~Spy()" << std::endl;
    }

    Spy& operator=(Spy&& other) {
        std::cout << "Spy(Spy&&)" << std::endl;

        return *this;
    }

    Spy& operator=(const Spy& other) {
        std::cout << "Spy(const Spy&)" << std::endl;

         return *this;
    }
private:
    int i = 0;
};

#define ECHO(expr) std::cout << std::endl << "Echo: " #expr << std::endl; expr

int main() {
    ECHO(auto a = Spy(1));
    ECHO(Spy b(a));

    ECHO(Spy c(std::move(b)));    // конструктор
                                    // перемещения
    ECHO(a = b); // копирование

    ECHO(c = std::move(b)); // перемещение

    ECHO(Spy d(Spy(2))); // перемещение

    ECHO(return 0);
} */

/* #include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>

class String {
public:
    String() = default;
    String(size_t n, char c);
    String(const char* data_);

    ~String();

    String(const String& other);
    String(const std::string& other);

    String(String&& other) noexcept;
    String(std::string&& other);

    String& operator=(const String& other);
    String& operator=(const std::string& other);

    String& operator=(String&& other) noexcept;
    String& operator=(std::string&& other);

    String& operator+(const String& other);

private:
    size_t size_ = 0;
    char* data_ = nullptr;
};

String getDate() {
    String date;
    date = std::move(std::string("18 September 2023"));
    return date;
}

int main() {
    std::vector<String> BigVector;
    String str = getDate();
    BigVector.push_back(std::move(str));
    return 0;
}


String::String(size_t n, char c) {
    data_ = new char[n + 1];
    for (size_t i = 0; i < n; i++) {
        data_[i] = c;
    }
    data_[n] = '\0';
}

String::String(const char* data) {
    std::cout << "copy constructor called" << std::endl;
    size_t n = strlen(data);
    data_ = new char[n + 1];
    for (size_t i = 0; i < n; i++) {
        data_[i] = data[i];
    }
    data_[n] = '\0';
    size_ = n;
}

String::~String() {
    //std::cout << "destructor called" << std::endl;
    delete[] data_;
}

String::String(const String& other) : size_(other.size_) {
    std::cout << "copy constructor called" << std::endl;
    data_ = new char[size_ + 1];
    for (size_t i = 0; i < other.size_; i++)
        data_[i] = other.data_[i];
    data_[other.size_] = '\0';
}

String::String(const std::string& other) : size_(other.size()) {
    std::cout << "copy constructor called" << std::endl;
    data_ = new char[size_ + 1];
    for (size_t i = 0; i < other.size(); i++) {
        data_[i] = other.data()[i];
    }
    data_[other.size()] = '\0';
}

String::String(String&& other) noexcept {
    std::cout << "move constructor called" << std::endl;
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
}

String::String(std::string&& other) : size_(other.size()) {
    std::cout << "move constructor called" << std::endl;

    data_ = new char[size_ + 1]; // +1 for the null terminator
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = other.data()[i];
    }
    data_[other.size()] = '\0';
}

String& String::operator=(const String& other) {
    if (this != &other) {
        std::cout << "copy assigment operator called" << std::endl;
        data_ = new char[other.size_ + 1];
        for (size_t i = 0; i < other.size_; i++) {
            data_[i] = other.data_[i];
        }
        data_[other.size_] = '\0';
    }
    return *this;
}

String& String::operator=(const std::string& other) {
    std::cout << "copy assigment operator called" << std::endl;
    data_ = new char[other.size() + 1];
    for (size_t i = 0; i < other.size(); i++) {
        data_[i] = other.c_str()[i];
    }
    data_[other.size()] = '\0';
    return *this;
}

String& String::operator=(String&& other) noexcept {
    std::cout << "move assigment operator called" << std::endl;
    if (this != &other) {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }
    return *this;
}

String& String::operator=(std::string&& other) {
    std::cout << "move assignment operator called" << std::endl;

    delete[] data_;  // Free existing data

    size_ = other.size();
    data_ = new char[size_ + 1]; // +1 for the null terminator

    for (size_t i = 0; i < size_; ++i) {
        data_[i] = other.data()[i];
    }
    data_[other.size()] = '\0';

    return *this;
}

String& String::operator+(const String& other) {
    size_t newLength = size_ + other.size_;
    char* newStr = new char[newLength + 1];
    for (size_t i = 0; i < size_; ++i) {
        newStr[i] = data_[i];
    }
    for (size_t i = size_; i < newLength; ++i) {
        newStr[i] = other.data_[i];
    }
    newStr[newLength] = '\0';
    delete[] data_;
    data_ = newStr;
    size_ = newLength;
    return *this;
} */

/* #include<iostream>
#include<vector>

struct String {
    String() = default;
	String(size_t n, char c);
	String(const String &other);
    
    ~String();
    
    String& operator=(const String& other);
    String& operator=(const std::string& other);

	size_t size_ = 0;
	char* data_ = nullptr;
};

String::String(size_t n, char c) {
    data_ = new char[n + 1];
    for (size_t i = 0; i < n; i++) {
        data_[i] = c;
    }
    data_[n] = '\0';
}

String::String(const String& other) : size_(other.size_) {
    std::cout << "copy constructor called" << std::endl;
    data_ = new char[size_ + 1];
    for (size_t i = 0; i < other.size_; i++)
        data_[i] = other.data_[i];
    data_[other.size_] = '\0';
}

String::~String() {
    //std::cout << "destructor called" << std::endl;
    delete[] data_;
}

String& String::operator=(const String& other) {
    if (this != &other) {
        std::cout << "copy assigment operator called" << std::endl;
        data_ = new char[other.size_ + 1];
        for (size_t i = 0; i < other.size_; i++) {
            data_[i] = other.data_[i];
        }
        data_[other.size_] = '\0';
    }
    return *this;
}

String& String::operator=(const std::string& other) {
    std::cout << "copy assigment operator called" << std::endl;
    data_ = new char[other.size() + 1];
    for (size_t i = 0; i < other.size(); i++) {
        data_[i] = other.c_str()[i];
    }
    data_[other.size()] = '\0';
    return *this;
}

String getDate() {
    String date;
    date = "18 September 2023";
    return date;
}

int main()
{
    std::vector<String> BigVector;
    String str = getDate();
    BigVector.push_back(str);
    return 0;
} */