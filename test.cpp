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
             if (lhs.rating == rhs.rating) {
                return lhs.relevance > rhs.relevance;
             } else {
                return lhs.rating > rhs.rating;
             }
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
/*
sort(observations.begin(), observations.end(),
     [](const AnimalObservation& lhs, const AnimalObservation& rhs) {
          return lhs.days_ago < rhs.days_ago
               || (lhs.days_ago == rhs.days_ago
               && lhs.health_level < rhs.health_level);
     }); 
*/



/* #include <iostream>

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