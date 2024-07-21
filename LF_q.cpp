#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    
    int n = 0;
    
    string val;
    
    vector<string> v;
    
    cin >> n;
    
    v.reserve(n);
    
    for (int i = 0; i < n; ++i) {
        
        cin >> val;
        
        v.push_back(val);
    }
    
    sort(begin(v), end(v), [](const string& l, const string& r) {
            return lexicographical_compare(
                begin(l), end(l),
                begin(r), end(r),
                
                [](char cl, char cr) { return tolower(cl) < tolower(cr); }
            );
        }
    );
    
    for (int i = 0; i < v.size(); ++i) {
        
        cout << v[i] << ' ';
    }
    
    cout << endl;
}