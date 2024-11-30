#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>

using namespace std;

class Base {
public:
    virtual ~Base() { std::cout << "Destructor of Base\n"; }
};

class Derived : public Base {
public:
    ~Derived() override { std::cout << "Destructor of Derived\n"; }
};

int main() {
    Base* ptr = new Derived();
    delete ptr; // Всё работает корректно!
}
