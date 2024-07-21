#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

int main() {
    const int queryCount = ReadLineWithNumber();

    vector<string> queries(queryCount);
    for (string& query : queries) {
        query = ReadLine();
    }
    const string buzzword = ReadLine();

    cout << count_if(queries.begin(), queries.end(), [buzzword](const string& query) {
            set<string> set_str;
            string word;

            for (const char c : query) {
                if (c == ' ') {
                    if (!word.empty()) {
                        set_str.insert(word);
                        word.clear();
                    }
                } else {
                    word += c;
                }
            }
            if (!word.empty()) {
                set_str.insert(word);
            }
            return (set_str.count(buzzword));
    });
    
    cout << endl;

}