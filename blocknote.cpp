#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

#include "log_duration.h"

using namespace std;

class RandomContainer {
public:
    void Insert(int val) {
        values_pool_.push_back(val);
        index_map_[val] = values_pool_.size() - 1;
    }
    void Remove(int val) {
        //values_pool_.erase(find(values_pool_.begin(), values_pool_.end(), val));
        int index_to_remove = index_map_[val];
        int last_element = values_pool_.back();
        values_pool_[index_to_remove] = last_element;
        index_map_[last_element] = index_to_remove;
        values_pool_.pop_back();
        index_map_.erase(val);
    }
    bool Has(int val) const {
        //return find(values_pool_.begin(), values_pool_.end(), val) != values_pool_.end();
        return index_map_.count(val) > 0;
    }
    int GetRand() const {
        uniform_int_distribution<int> distr(0, values_pool_.size() - 1);
        int rand_index = distr(engine_);
        return values_pool_[rand_index];
    }

private:
    vector<int> values_pool_;
    unordered_map<int, int> index_map_;
    mutable mt19937 engine_;
};

int main() {
    RandomContainer container;
    int query_num = 0;
    cin >> query_num;
    {
        LOG_DURATION("Requests handling"s);
        for (int query_id = 0; query_id < query_num; query_id++) {
            string query_type;
            cin >> query_type;
            if (query_type == "INSERT"s) {
                int value = 0;
                cin >> value;
                container.Insert(value);
            } else if (query_type == "REMOVE"s) {
                int value = 0;
                cin >> value;
                container.Remove(value);
            } else if (query_type == "HAS"s) {
                int value = 0;
                cin >> value;
                if (container.Has(value)) {
                    cout << "true"s << endl;
                } else {
                    cout << "false"s << endl;
                }
            } else if (query_type == "RAND"s) {
                cout << container.GetRand() << endl;
            }
        }
    }
}