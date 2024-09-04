#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <string>

using namespace std;

template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(RandomIt range_begin, RandomIt range_end, string prefix) {
    // Все строки, начинающиеся с '<prefix>', больше или равны строке "<prefix>"
    auto left = lower_bound(range_begin, range_end, prefix);

    // Составим следующий в алфавите символ.
    // Не страшно, если prefix = 'z':
    //в этом случае мы получим следующий за 'z' символ в таблице символов

    string next_prefix = prefix;
    next_prefix[next_prefix.size() - 1] = next_prefix[next_prefix.size() - 1] + 1;

    // Строка "<next_prefix>" в рамках буквенных строк
    // является точной верхней гранью
    // множества строк, начнающихся с '<prefix>'
    auto right = lower_bound(range_begin, range_end, next_prefix);

    return {left, right};
}

int main() {
    const vector<string> sorted_strings = {"moscow", "motovilikha", "murmansk"};
    const auto mo_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mo");
    for (auto it = mo_result.first; it != mo_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;
    const auto mt_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mt");
    cout << (mt_result.first - begin(sorted_strings)) << " " << (mt_result.second - begin(sorted_strings)) << endl;
    const auto na_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "na");
    cout << (na_result.first - begin(sorted_strings)) << " " << (na_result.second - begin(sorted_strings)) << endl;
    return 0;
}