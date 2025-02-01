#include <iostream>

int main() {
#ifdef __GNUC__
    std::cout << "GCC version: " << __VERSION__ << std::endl;
#endif
#ifdef __clang__
    std::cout << "Using Clang instead of GCC" << std::endl;
#endif
    return 0;
}