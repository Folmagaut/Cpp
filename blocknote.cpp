#include <iostream>

using namespace std;

bool IsPowOfTwo(int number) {
    
    if (number == 0) {
        return false;
    }
    
    if (number == 1) {
        return true;
    }

    if (number % 2 == 0) {
        return IsPowOfTwo(number / 2);
    } else {
        return false;
    }
}

int main() {
    int result = IsPowOfTwo(4);
    cout << result << endl;
    return 0;
}