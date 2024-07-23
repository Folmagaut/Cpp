#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

int CountAndAddNewDogs(const vector<string>& new_dogs, const map<string, int>& max_amount,
                       map<string, int>& shelter) {
    
    return count_if(new_dogs.begin(), new_dogs.end(), [&max_amount, &shelter](const string& n_d) {
        if (shelter[n_d] == max_amount.at(n_d)) {
            return false;
        } else if (shelter.count(n_d) == 0) {
            shelter[n_d] = 1;
            return true;
        } else if (shelter[n_d] < max_amount.at(n_d)) {
            ++shelter[n_d];
            return true;
        }
        return false;       
        }
    );
    
}

int main() {
    map<string, int> shelter {
        {"shepherd"s, 1},
        {"corgi"s, 3},
    };

    const map<string, int> max_amount {
        {"shepherd"s, 2},
        {"corgi"s, 3},
        {"shiba inu"s, 1},
    };

    cout << CountAndAddNewDogs({"shepherd"s, "shiba inu"s, "shiba inu"s, "corgi"s}, max_amount, shelter) << endl;
}