#include <algorithm>
#include<cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double EPSILON = 1e-6;

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED

};
string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;

    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
    int rating;
   
};
void PrintDocument(const Document& document) {
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating
        << " }"s << endl;
}

void PrintMatchDocumentResult(int document_id, const vector<string>& words, DocumentStatus status) {
    cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << static_cast<int>(status) << ", "s
        << "words ="s;
    for (const string& word : words) {
        cout << ' ' << word;
    }
    cout << "}"s << endl;
}

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }
    int GetDocumentCount() const {
        return document_count_;
    }
    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
        Query qr = ParseQuery(raw_query);

        vector<string>wor;
        for (const auto& word : qr.plus_words) {
            for (const auto& now : documents_.at(word)) {
                if (now.first == document_id && documents_.count(word) > 0) {

                    wor.push_back(word);
                }
            }
        }

        for (const auto& word : qr.minus_words) {
            for (const auto& now : documents_.at(word)) {
                if (now.first == document_id && documents_.count(word) > 0) {
                    wor.clear();
                    return make_tuple(wor, document_status_.at(document_id));
                        
                }
            }
        }
        return make_tuple(wor, document_status_.at(document_id));
    }
    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& document_rating) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        double size = 1. / words.size();
        for (const auto& word : words) {
            //добавляем TF каждого слова в наш documents_
            documents_[word][document_id] += size;

        }
        document_rating_.insert({ document_id, ComputeAverageRating(document_rating) });
        document_status_.insert({ document_id,status });
        document_count_++;
    }
    vector<Document> FindTopDocuments(const string& raw_query) {
        vector<Document>mv;
        mv = FindTopDocuments(raw_query, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL; });
        return mv;
    }
    
    template<typename funct>
    vector<Document> FindTopDocuments(const string& raw_query, funct func) const {
        
        Query query_words = ParseQuery(raw_query);
        
        vector<Document> matched_documents =  FindAllDocuments(query_words,func);

        
        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                    return lhs.rating > rhs.rating;
                }
                else {
                    return lhs.relevance > rhs.relevance;
                }
            });
        
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
       
       return matched_documents;
          
    }

private:

    int document_count_ = 0;
    map<string, map<int, double>> documents_;
    
    set<string> stop_words_;
    map<int, int>document_rating_;
    map<int, DocumentStatus>document_status_;
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }
    static int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {

            return 0;

        }
        int rating_sum = 0;
        for (const int rating : ratings) {
            rating_sum += rating;
        }

        return rating_sum / static_cast<int>(ratings.size());

    }
    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }


    //именованная функция для подсчета IDF
    double GetIDF(const string& str) const {
        double IDF = log(static_cast<double>(document_count_) / documents_.at(str).size());
        return IDF;
    }
    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return { text, is_minus, IsStopWord(text) };
    }

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                }
                else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }
    template<typename func>
    vector<Document> FindAllDocuments(const Query& query_words,func fun) const {
       
        vector<Document> matched_documents;
        map<int, double>document_to_relevance;
        
        for (const auto& word : query_words.plus_words) {
            if (documents_.count(word) > 0) {
                for (const auto& now : documents_.at(word)) {
                    //считаем IDF каждого слова  через именованную  функцию
                    //складываем relevance каждого слова в запросе(relevance=формула TF-IDF)
                    //now.first-id,now.second-relevance(TF)
                    document_to_relevance[now.first] += (now.second * GetIDF(word));

                }
            }

        }

        for (const auto& word : query_words.minus_words) {
            if (documents_.count(word) > 0) {
                for (const auto& now : documents_.at(word)) {
                    document_to_relevance.erase(now.first);
                }
            }
        }
        
        for (const auto& document : document_to_relevance) {
            if (fun(document.first, document_status_.at(document.first), document_rating_.at(document.first))) {
                matched_documents.push_back({ document.first,document.second,document_rating_.at(document.first)});
               
            }
            
        }
        
        return matched_documents;
    }


};



int main() {
    SearchServer search_server;
    search_server.SetStopWords("и в на"s);
    search_server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
    search_server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::BANNED, { 9 });
    cout << "ACTUAL by default:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s)) {
        PrintDocument(document);
    }
    cout << "ACTUAL:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL; })) {
        PrintDocument(document);
    }
    cout << "Even ids:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
        PrintDocument(document);
    }
    return 0;
}