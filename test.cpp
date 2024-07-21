#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

vector<string> SplitIntoWords(const string& s) {
    vector<string> words;
    string word;
        for (const char c : s) {
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
        words.erase(words.begin());
        return words;
    }

string ToLowerCase(string s) {
    for (char& ch : s) {
        ch = tolower(static_cast<unsigned char>(ch));
    }
    return s;
}

int main() {
    
    string str;
    getline(cin, str);

    vector<string> v_s = SplitIntoWords(str);

    sort(v_s.begin(), v_s.end(),
    [](const string& left, const string& right) {
        return ToLowerCase(left) < ToLowerCase(right);
        });

    for (const string& s : v_s) {
        cout << s << " "s;
    }
    cout << " "s << endl;

}