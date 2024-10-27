#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

class MoneyBox {
public:
    void PushCoin(int value) {
        assert(value >= 0 && value <= 5000);
        ++counts_[value];
    }

    void PrintCoins(ostream& out) const {
        for (int i = 0; i < counts_.size(); ++i) {
            if (counts_[i] > 0) {
                out << i << ": "s << counts_[i] << endl;
            }
        }
    }

private:
    // создаём вектор размера 5001 для того, чтобы последний
    // индекс соответствовал максимальному номиналу 5000
    vector<int> counts_ = vector<int>(5001);
};

ostream& operator<<(ostream& out, const MoneyBox& cash) {
    cash.PrintCoins(out);
    return out;
}

int main() {
    MoneyBox cash;

    int value;
    while (cin >> value) {
        cash.PushCoin(value);
    }

    cout << cash << endl;
}

/*
// файл wordstat.cpp

#include "log_duration.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

using namespace std;

vector<pair<string, int>> GetSortedWordCounts(vector<string> words) {
    unordered_map<string, int> counts_map;

    {
        LOG_DURATION("Filling"s);

        for (auto& word : words) {
            ++counts_map[move(word)];
        }
    }

    {
        LOG_DURATION("Moving & sorting"s);

        vector<pair<string, int>> counts_vector(move_iterator(counts_map.begin()), move_iterator(counts_map.end()));
        sort(counts_vector.begin(), counts_vector.end(), [](const auto& l, const auto& r) {
            return l.second > r.second;
        });

        return counts_vector;
    }
}

int main() {
    vector<string> words;
    string word;
    ifstream file("canterbury.txt");

    while (file >> word) {
        words.push_back(word);
    }

    auto counts_vector = GetSortedWordCounts(move(words));

    cout << "Слово - Количество упоминаний в тексте"s << endl;
    // выводим первые 10 слов
    for (auto [iter, i] = tuple(counts_vector.begin(), 0); i < 10 && iter != counts_vector.end(); ++i, ++iter) {
        cout << iter->first << " - "s << iter->second << endl;
    }
}


#include <fstream>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include "log_duration.h"

using namespace std;

vector<pair<string, int>> GetSortedWordCounts(vector<string> words) {
    map<string, int> counts_map;

    for (auto& word : words) {
        if (word.size() == 5) {
            ++counts_map[move(word)];
        }
    }

    vector<pair<string, int>> counts_vector(move_iterator(counts_map.begin()), move_iterator(counts_map.end()));
    sort(counts_vector.begin(), counts_vector.end(), [](const auto& l, const auto& r) {
        return l.second > r.second;
    });

    return counts_vector;
}

int main() {
    vector<string> words;
    string word;
    ifstream file("canterbury.txt");
    {
    LOG_DURATION("Total time"s);
    while (file >> word) {
        words.push_back(word);
    }

    auto counts_vector = GetSortedWordCounts(move(words));
    
    cout << "Слово - Количество упоминаний в тексте"s << endl;
    // выводим первые 10 слов
    for (auto [iter, i] = tuple(counts_vector.begin(), 0); i < 10 && iter != counts_vector.end(); ++i, ++iter) {
        cout << iter->first << " - "s << iter->second << endl;
    }
    }
} */