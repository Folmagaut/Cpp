#include "log_duration.h"
#include <algorithm>
#include <array>
#include <vector>

using namespace std;

vector<int> BuildVector(int i) {
    return {i, i + 1, i + 2, i + 3, i + 4};
}

array<int, 5> BuildArray(int i) {
    return {i, i + 1, i + 2, i + 3, i + 4};
}

const int COUNT = 1000000;

int main() {
    {
        LOG_DURATION("vector");
        for (int i = 0; i < COUNT; ++i) {  // C * N
            auto numbers = BuildVector(i);
        }
    }
    {
        LOG_DURATION("array");
        for (int i = 0; i < COUNT; ++i) {  // C * N * logN
            auto numbers = BuildArray(i);
            sort(begin(numbers), end(numbers));
        }
    }

    return 0;
}