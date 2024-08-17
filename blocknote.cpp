#include <iostream>
#include <vector>
#include <set>
#include <string>

using namespace std;

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

/* template <typename Container>
vector<string> SplitIntoWords(const Container& vec_or_set) {

    set<string> set_of_words;
    for (const auto& str : vec_or_set) {
        vector<string> words = SplitIntoWords(str);
        for (const string& word : words) {
            set_of_words.insert(word);
        }
    }
    
    vector<string> words(set_of_words.begin(), set_of_words.end());
    return words;
} */

class SearchServer {
private:
    set<string> stop_words_; // Используем set для хранения стоп-слов

public:
    template <typename Container>
    SearchServer(const Container& vec_or_set_of_stop_words) : stop_words_({}) {
        for (const auto& str : vec_or_set_of_stop_words) {
            vector<string> words = SplitIntoWords(str);
            for (const string& word : words) {
                stop_words_.insert(word);
            }
        }
    }

    SearchServer(const string& text) : stop_words_({}) {
        for (const auto& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void PrintStopWords() const {
        for (const auto& word : stop_words_) {
            cout << word << " ";
        }
        cout << endl;
    }
};

int main() {
    // Инициализация поисковой системы с использованием вектора строк
    const vector<string> stop_words_vector = {"и"s, "в"s, "на"s, ""s, "в"s};
    SearchServer search_server1(stop_words_vector);
    search_server1.PrintStopWords();

    // Инициализация поисковой системы с использованием множества строк
    const set<string> stop_words_set = {"и"s, "в"s, "на"s};
    SearchServer search_server2(stop_words_set);
    search_server2.PrintStopWords();

    // Инициализация поисковой системы с использованием строки со стоп-словами, разделенными пробелами
    SearchServer search_server3("  и  в на   "s);
    search_server3.PrintStopWords();

    return 0;
}
