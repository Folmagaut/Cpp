#include <iostream>
#include <map>
#include <string>

using namespace std;

// Определение функции Print для словарей
template <typename KeyType, typename ValueType>
ostream& operator<<(ostream& out, const map<KeyType, ValueType>& container) {
    out << "<";
    bool is_first = true;
    for (const auto& pair : container) {
        if (!is_first) {
            out << ", ";
        }
        is_first = false;
        out << "(" << pair.first << ", " << pair.second << ")";
    }
    out << ">";
    return out;
}

int main() {
    // Пример использования
    map<string, int> cat_ages = {
        {"Мурка", 10}, 
        {"Белка", 5},
        {"Георгий", 2}, 
        {"Рюрик", 12}
    };
    cout << cat_ages << endl;
    return 0;
}
