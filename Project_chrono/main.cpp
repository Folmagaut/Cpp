#include "log_duration.h"

#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

using namespace std;

vector<float> ComputeAvgTemp(const vector<vector<float>>& measures) {
    // место для вашего решения
    if (measures.empty()) {
        return {};
    }
    //const int num_days = measures.size();
    //const int measures_per_day = measures[0].size();
    vector<float> avg_vec(measures.size());
    vector<int> positive_temp_counter(measures.size());
    for (auto vec_of_floats : measures) {
        for (size_t i = 0; i < vec_of_floats.size(); ++i) {
            avg_vec[i] += (vec_of_floats[i] > 0 ? (vec_of_floats[i]) : 0);
            positive_temp_counter[i] += (vec_of_floats[i] > 0 ? 1 : 0);
        }
    }

    for (size_t i = 0; i < measures[0].size(); ++i) {
        float div = (positive_temp_counter[i] != 0 ? 1.f/positive_temp_counter[i] : 0);
        avg_vec[i] *= div;
    }
    return avg_vec;
}

vector<float> GetRandomVector(int size) {
    static mt19937 engine;
    uniform_real_distribution<float> d(-100, 100);

    vector<float> res(size);
    for (int i = 0; i < size; ++i) {
        res[i] = d(engine);
    }

    return res;
}

int main() {
    vector<vector<float>> data;
    data.reserve(5000);

    for (int i = 0; i < 5000; ++i) {
        data.push_back(GetRandomVector(5000));
    }

    vector<float> avg;
    {
        LOG_DURATION("ComputeAvgTemp"s);
        avg = ComputeAvgTemp(data);
    }

    cout << "Total mean: "s << accumulate(avg.begin(), avg.end(), 0.f) / avg.size() << endl;
}