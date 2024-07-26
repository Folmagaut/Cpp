#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>


using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

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
        } else {
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
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        int count_all_words_in_document = words.size();
        double once_met_word_in_document_tf = 1.0 / count_all_words_in_document;
        for (const string& word : words) {
            documents_[word][document_id] += once_met_word_in_document_tf;
        }
        ++document_count_;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const set<string> query_words = ParseQuery(raw_query);
        const set<string> minus_words = ParseQueryMinusWords(raw_query);
        auto matched_documents = FindAllDocuments(query_words, minus_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
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

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
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

    set<string> ParseQuery(const string& text) const {
        set<string> query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            query_words.insert(word);
        }
        return query_words;
    }

    set<string> ParseQueryMinusWords(const string& text) const {
        set<string> minus_words;
        for (const string& minus_word : SplitIntoWordsNoStop(text)) {
            if (minus_word[0] == '-') {
                minus_words.insert(minus_word.substr(1));
            }
        }
        return minus_words;
    }

    double CountIdf (const string& word) const {
        double idf = log(static_cast<double>(document_count_) / documents_.at(word).size());
        return idf;
    }

    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const {
        vector<Document> matched_documents;
        Document doc;
        map<int, double> id_tf_idf;

        for (const string& word : query_words) {
            if (documents_.count(word)) {    
                double idf = CountIdf(word);
                for (auto& id_tf : documents_.at(word)) {
                    double tf_idf = idf * id_tf.second;
                    id_tf_idf[id_tf.first] += tf_idf;
                }
            }  
        }

        for (const string& minus_word : minus_words) {
            if (documents_.count(minus_word)) {
                for (auto& id_tf : documents_.at(minus_word)) {
                id_tf_idf.erase(id_tf.first);
                }
            }
        }

        for (const auto& i: id_tf_idf) {
            doc.id = i.first;
            doc.relevance = i.second;
            matched_documents.push_back(doc);
            }
        
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }
    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();

    for (const auto& i : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << i.id << ", "
             << "relevance = "s << i.relevance << " }"s << endl;
    }
}