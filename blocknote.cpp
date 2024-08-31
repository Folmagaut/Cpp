<<<<<<< HEAD
#include <iostream>

using namespace std;

bool IsPowOfTwo(int number) {
    
    if (number == 0) {
        return false;
    }
    
    if (number == 1) {
        return true;
    }

    if (number % 2 == 0) {
        return IsPowOfTwo(number / 2);
    } else {
        return false;
    }
}

int main() {
    int result = IsPowOfTwo(4);
    cout << result << endl;
    return 0;
}
=======
#include <algorithm>
#include <iostream> 
#include <string> 
#include <vector> 

using namespace std; 

struct Lang { 
    string name; 
    int age; 
};

int main() { 
    vector<Lang> langs = {{"Python"s, 29}, {"Java"s, 24}, {"C#"s, 20}, {"Ruby"s, 25}, {"C++"s, 37}};

    auto it = find_if(langs.begin(), langs.end(), [](const Lang& lang) { return lang.name[0] == 'J'; });

    cout << it->name << ", "s << it->age << endl;

    return 0;
}
>>>>>>> a82fee3554911372c5789de0008120c3ee76852e
