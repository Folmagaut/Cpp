#include "string_processing.h"
#include <algorithm>
#include <stdexcept>

using namespace std;

bool IsValidWord(const string& word) {
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
        });
    }

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                if (!IsValidWord(word)) {
                    throw invalid_argument("Invalid_argument");
                }
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        if (!IsValidWord(word)) {
            throw invalid_argument("Invalid_argument");
        }
        words.push_back(word);
    }
    return words;
}