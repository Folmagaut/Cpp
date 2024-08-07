#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct Animal {
    string name;
    int age;
    double weight;
};

enum class AnimalSortKey {
    AGE,     // по полю age
    WEIGHT,  // по полю weight
    RELATIVE_WEIGHT  // по weight / age
};

/* template <typename Container, typename KeyMapper>
void SortBy(Container& container, KeyMapper key_mapper, bool reverse = false) {
    sort(container.begin(), container.end(), [key_mapper, reverse](const auto& lhs, const auto& rhs) {
        if (reverse) {
            return key_mapper(lhs) > key_mapper(rhs);
        } else {
            return key_mapper(lhs) < key_mapper(rhs);
        }
    });
} */

template <typename Container, typename KeyMapper>
void SortBy(Container& container, KeyMapper key_mapper, bool reverse = false) {
        // если KeyMapper — это AnimalSortKey...
    if constexpr (is_same_v<KeyMapper, AnimalSortKey>) {
        switch (key_mapper) {
            case AnimalSortKey::AGE:
                return SortBy(container, [](const auto& x) { return x.age; }, reverse);
            case AnimalSortKey::WEIGHT:
                return SortBy(container, [](const auto& x) { return x.weight; }, reverse);
            case AnimalSortKey::RELATIVE_WEIGHT:
                return SortBy(container, [](const auto& x) { return x.weight / x.age; }, reverse);
        }
                // вышли из функции, остальное снаружи if
    } else {
        if (reverse) {
            sort(container.begin(), container.end(),
             [key_mapper](const auto& lhs, const auto& rhs) {
                return key_mapper(lhs) > key_mapper(rhs);
             });
        } else {
            sort(container.begin(), container.end(),
             [key_mapper](const auto& lhs, const auto& rhs) {
                return key_mapper(lhs) < key_mapper(rhs);
             });
        }
    }

}

void PrintNames(const vector<Animal>& animals) {
    for (const Animal& animal : animals) {
        cout << animal.name << ' ';
    }
    cout << endl;
}

int main() {
    vector<Animal> animals = {
        {"Мурка"s,   10, 5},
        {"Белка"s,   5,  1.5},
        {"Георгий"s, 2,  4.5},
        {"Рюрик"s,   12, 3.1},
    };
    PrintNames(animals);
    SortBy(animals, [](const Animal& animal) { return animal.name; }, true);
    PrintNames(animals);
    SortBy(animals, [](const Animal& animal) { return animal.weight; });
    PrintNames(animals);
    return 0;
}

/*
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;
template <typename AnyType>
ostream& operator<<(ostream& out, const set<AnyType>& container);
template <typename AnyType>
ostream& operator<<(ostream& out, const vector<AnyType>& container);
template <typename AnyKey, typename AnyValue>
//ostream& operator<<(ostream& out, const pair<AnyKey, AnyValue>& container);
//template <typename AnyKey, typename AnyValue>
ostream& operator<<(ostream& out, const map<AnyKey, AnyValue>& container);

template <typename AnyKey, typename AnyValue>
ostream& operator<<(ostream& out, const pair<AnyKey, AnyValue>& container) {
    out << "(";
    out << container.first;
    out << ", ";
    out << container.second;    
    out << ")";
    return out;
}

//template <typename AnyKey, typename AnyValue>
//void PrintPair(ostream& out, const pair<AnyKey, AnyValue>& container) {
 //   out << "(";
 //   out << container.first;
  //  out << ", ";
  //  out << container.second;    
  //  out << ")";
//}

template <typename Container>
void Print(ostream& out, const Container& container) {
    bool is_first = true;
    for (const auto& element : container) {
        if (!is_first) {
            out << ", "s;
        }
        is_first = false;
        out << element;
    }
}

template <typename AnyType>
ostream& operator<<(ostream& out, const set<AnyType>& container) {
    out << "{";
    Print(out, container);
    out << "}";
    return out;
}

template <typename AnyType>
ostream& operator<<(ostream& out, const vector<AnyType>& container) {
    out << "[";
    Print(out, container);
    out << "]";
    return out;
}

template <typename AnyKey, typename AnyValue>
ostream& operator<<(ostream& out, const map<AnyKey, AnyValue>& container) {
    out << "<";
    Print(out, container);
    out << ">";
    return out;
}

int main() {
const set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
cout << cats << endl;
const vector<int> ages = {10, 5, 2, 12};
cout << ages << endl;
const map<string, int> cat_ages = {
    {"Мурка"s, 10}, 
    {"Белка"s, 5},
    {"Георгий"s, 2}, 
    {"Рюрик"s, 12}
};
cout << cat_ages << endl;
}

//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>

using namespace std;

template <typename AnyType>

ostream& operator<<(ostream& out, const vector<AnyType>& container) {
    size_t i = 0;
    for (const AnyType& element : container) {
        ++i;
        if (container.size() == i) {
            out << element;
        } else {
            out << element << ", "s;
        }
    }
    return out;
}  
   
int main() {
    const vector<int> ages = {10, 5, 2, 12};
    cout << ages << endl;
    return 0;
}

//////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

template <typename Document, typename Term>

vector<double> ComputeTfIdfs (Document documents, Term term) {
    vector<double> tf_idf;
    double tf = 0;
    double idf = 0;
    int idf_count = 0;
    double inv_term_count = 0;
    for (const auto& document : documents) {
        if (!document.empty() && (count(document.begin(), document.end(), term)) > 0) {
            inv_term_count = 1.0 / document.size();
            tf = inv_term_count * count(document.begin(), document.end(), term);
            ++idf_count;
        } else {
            tf = 0;
        }
        tf_idf.push_back(tf);
    }
    if (idf_count > 0) {
        idf = log(static_cast<double>(documents.size()) / static_cast<double>(idf_count));
    }
    for (auto& tf : tf_idf) {
        tf = tf * idf;
    }
        
    return tf_idf;
}

int main() {
    const vector<vector<string>> documents = {
        {"белый"s, "кот"s, "и"s, "модный"s, "ошейник"s},
        {"пушистый"s, "кот"s, "пушистый"s, "хвост"s},
        {"ухоженный"s, "пёс"s, "выразительные"s, "глаза"s},
    };
    const auto tf_idfs = ComputeTfIdfs(documents, "кот"s);
    for (const double tf_idf : tf_idfs) {
        cout << tf_idf << " "s;
    }
    cout << endl;
    return 0;
}

////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;
template <typename Term>
map<Term, int> ComputeTermFreqs(const vector<Term>& terms) {
    map<Term, int> term_freqs;
    for (const Term& term : terms) {
        ++term_freqs[term];
    }
    return term_freqs;
}

struct Animal {
    string name;
    int age;
    int freq;
};

pair<string, int> FindMaxFreqAnimal(const vector<pair<string, int>>& animals) {
    // верните животного с максимальной частотой
    map<pair<string, int>, int> map_of_animals_and_freqs;
    vector<Animal> animals_and_freqs;
    pair<string, int> max_freq_animal;
    for (const auto& animal : animals) {
        ++map_of_animals_and_freqs[animal];
    }

    for (const auto& [name_and_age, freq] : map_of_animals_and_freqs) {
        animals_and_freqs.push_back({name_and_age.first, name_and_age.second, freq});
    }
    
    sort(animals_and_freqs.begin(), animals_and_freqs.end(),
        [] (const Animal& lhs, const Animal& rhs) {
            if (lhs.freq == rhs.freq) {
                return lhs.name < rhs.name;
            } else {
                return lhs.freq > rhs.freq;
            }
    });
    max_freq_animal.first = animals_and_freqs[0].name;
    max_freq_animal.second = animals_and_freqs[0].age;
    return max_freq_animal;
}

int main() {
    const vector<pair<string, int>> animals = {
        {"Murka"s, 5},  // 5-летняя Мурка
        {"Belka"s, 6},  // 6-летняя Белка
        {"Murka"s, 7},  // 7-летняя Мурка не та же, что 5-летняя!
        {"Murka"s, 5},  // Снова 5-летняя Мурка
        {"Belka"s, 6},  // Снова 6-летняя Белка
    };
    const pair<string, int> max_freq_animal = FindMaxFreqAnimal(animals);
    cout << max_freq_animal.first << " "s << max_freq_animal.second << endl;
}

////////////////////////////////////////////////////////
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

map<string, int> ComputeTermFreqs(const vector<string>& terms) {
    // реализуйте функцию
    map<string, int> words_term_freqs;
    for (const string& word : terms) {
        ++words_term_freqs[word];
    }
    return words_term_freqs;
}

int main() {
    const vector<string> terms = {"first"s, "time"s, "first"s, "class"s};
    for (const auto& [term, freq] : ComputeTermFreqs(terms)) {
        cout << term << " x "s << freq << endl;
    }
    // вывод:
    // class x 1
    // first x 2
    // time x 1
}

///////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
    auto MakeKey() const {
        return tuple(status, rating * -1, relevance* -1);
    }
};

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](auto& lhs, auto& rhs) {
            return lhs.MakeKey() < rhs.MakeKey();
         });
}

int main() {
    vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4}, {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4}, {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << endl;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
};

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
            return tuple(lhs.status, -lhs.rating, -lhs.relevance) < tuple(rhs.status, -rhs.rating, -rhs.relevance);
         });
}

int main() {
    vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4}, {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4}, {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << endl;
    }

    return 0;
}
////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
};

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
            if (lhs.status == rhs.status) {
                return pair(lhs.rating, lhs.relevance) > pair(rhs.rating, rhs.relevance);
            } else {
                return (lhs.status < rhs.status);
            }
            
         });
}

int main() {
    vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4}, {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4}, {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << endl;
    }

    return 0;
}

///////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct Document {
    int id;
    double relevance;
    int rating;
};

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
                return pair(lhs.rating, lhs.relevance) > pair(rhs.rating, rhs.relevance);
         });
}

int main() {
    vector<Document> documents = {{100, 0.5, 4}, {101, 1.2, 4}, {102, 0.3, 5}};
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << document.relevance << ' ' << document.rating << endl;
    }

    return 0;
}

sort(observations.begin(), observations.end(),
     [](const AnimalObservation& lhs, const AnimalObservation& rhs) {
          return lhs.days_ago < rhs.days_ago
               || (lhs.days_ago == rhs.days_ago
               && lhs.health_level < rhs.health_level);
     }); 



///////////////////////////////////////////////////////////////////////////
#include <iostream>

using namespace std;

class SearchServer {
    public:
        // перенесите сюда DocumentStatus
        enum class DocumentStatus {
        ACTUAL,
        IRRELEVANT,
        BANNED,
        REMOVED,
    };
};

int main() {
    // выведите численное значение статуса BANNED
    int banned = static_cast<int>(SearchServer::DocumentStatus::BANNED);
    cout << banned<< endl;
    return 0;
}


////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>
//#include <cstdint>
 
using namespace std;
 
struct Document {
    int id;
    int rating;
};
 
void PrintDocuments(vector<Document> documents, size_t skip_start, size_t skip_finish) {
    //size_t doc_size = documents.size();
    if (documents.size() > 1) {
        sort(documents.begin(), documents.end(), [](const Document& lhs, const Document& rhs) {
        return lhs.rating > rhs.rating;
        });
 
        for (size_t i = 0; i < skip_finish; ++i) {
            documents.pop_back();
            if (documents.size() == 0) {
            break;
            }
        }
    
        for (size_t i = 0; i < skip_start; ++i) {
            documents.erase(documents.begin());
            if (documents.size() == 0) {
                break;
            }
        }
        
        if (documents.size() > 0) {
            for (auto i : documents) {
                cout << "{ id = "s << i.id << ", "s << "rating = "s << i.rating << " }"s << endl;
            }
        }
    }
}
 
int main() {
    PrintDocuments({{100, 5}, {101, 7}, {102, -4}, {103, 9}, {104, 1}}, 1, 2);
}

/////////////////////////////////////////////////
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

///////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int CountAndAddNewDogs(const vector<string>& new_dogs,
                       const map<string, int>& max_amount,
                       map<string, int>& shelter) {
    return count_if(new_dogs.begin(), new_dogs.end(),
        [&max_amount, &shelter](const string& name) {
            int& current_amount = shelter[name];
            if (current_amount < max_amount.at(name)) {
                ++current_amount;
                return true;
            } else {
                return false;
            }
        });
}

int main() {
    map<string, int> shelter {
        {"shepherd"s, 1},
        {"corgi"s, 3},
    };

    map<string, int> max_amount {
        {"shepherd"s, 2},
        {"corgi"s, 3},
        {"shiba inu"s, 1},
    };

    cout << CountAndAddNewDogs({"shepherd"s, "shiba inu"s, "shiba inu"s, "corgi"s}, max_amount, shelter) << endl;
}
*/