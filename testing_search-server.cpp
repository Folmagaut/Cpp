#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

/* Подставьте вашу реализацию класса SearchServer сюда */
const int MAX_RESULT_DOCUMENT_COUNT = 5;

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
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    }

    template <typename KeyFilter>
    vector<Document> FindTopDocuments(const string& raw_query, KeyFilter key_filter) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, key_filter);
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                     return lhs.rating > rhs.rating;
                 } else {
                     return lhs.relevance > rhs.relevance;
                 }
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus req_status) const {
        vector<Document> matched_documents_by_default = FindTopDocuments(raw_query, [req_status](int document_id, DocumentStatus status, int rating) {
                                                        return status == req_status;
                                                        });
        return matched_documents_by_default;
    }
    
    vector<Document> FindTopDocuments(const string& raw_query) const {
        vector<Document> matched_documents_by_default = FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
        return matched_documents_by_default;
    }

    int GetDocumentCount() const {
        return documents_.size();
    }

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        return {matched_words, documents_.at(document_id).status};
    }

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;

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
        return {text, is_minus, IsStopWord(text)};
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
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    // Existence required
    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }
    template <typename KeyFilter>
    vector<Document> FindAllDocuments(const Query& query, KeyFilter key_filter) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word)) {
                if (key_filter(document_id, documents_.at(document_id).status, documents_.at(document_id).rating) == true) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto &[document_id, tf_idf] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }
};

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "rating = "s << document.rating
         << " }"s << endl;
}

template <typename T>
ostream& operator<<(ostream& output, const vector<T>& items) {
    output << "["s;
    bool first_item = true;
    for (const T& item : items) {
        if (!first_item) {
            output << ", "s;
        }
        output << item;
        first_item = false;
    }
    output << "]"s;
    return output;
}

template <typename T>
ostream& operator<<(ostream& output, const set<T>& items) {
    output << "{"s;
    bool first_item = true;
    for (const T& item : items) {
        if (!first_item) {
            output << ", "s;
        }
        output << item;
        first_item = false;
    }
    output << "}"s;
    return output;
}

template <typename K, typename V>
ostream& operator<<(ostream& output, const map<K, V>& items) {
    output << "{"s;
    bool first_item = true;
    for (const auto& [key, value] : items) {
        if (!first_item) {
            output << ", "s;
        }
        output << key << ": "s << value;
        first_item = false;
    }
    output << "}"s;
    return output;
}

/*
   Подставьте сюда вашу реализацию макросов
   ASSERT, ASSERT_EQUAL, ASSERT_EQUAL_HINT, ASSERT_HINT и RUN_TEST
*/

void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
                const string& hint) {
    if (!value) {
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "Assert("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
                     const string& func, unsigned line, const string& hint) {
    if (t != u) {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

template <typename TestFunc>
void RunTestImpl(const TestFunc& func, const string& test_name) {
    func();
    cerr << test_name << " OK"s << endl;
}

#define ASSERT(a) AssertImpl((a), #a, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_HINT(a, hint) AssertImpl((a), #a, __FILE__, __FUNCTION__, __LINE__, (hint))
#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))
#define RUN_TEST(func) RunTestImpl(func, #func)

// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(),
                    "Stop words must be excluded from documents"s);
    }
}

/*
Разместите код остальных тестов здесь
*/
void TestAddingDocsToAddDocumentFunction() {
    const int doc_id = 0;
    const string content = "smoke on the water"s;
    const vector<int> ratings = {1, 2, 3};
    const int doc_id1 = 1;
    const string content1 = "fire in the sky"s;
    const vector<int> ratings1 = {1, 2, 3};

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        const auto found_docs = server.FindTopDocuments("fire"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id1);
    }

    {
        SearchServer server;
        ASSERT_HINT(server.GetDocumentCount() == 0,
                    "Add any documents first"s);
    }

}

void TestExcludeMinusWordsFromResults() {
    const int doc_id = 0;
    const string content = "fish in the sea"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("fish"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("-fish"s).empty(),
                    "Docs with minus words must be excluded from finding"s);
    }
}

void TestComplianceDocumentsToQuery() {
    const int doc_id = 0;
    const string content = "smoke on the water"s;
    const vector<int> ratings = {1, 2, 3};
    const int doc_id1 = 1;
    const string content1 = "fire in the sky"s;
    const vector<int> ratings1 = {1, 2, 3};

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        const auto found_docs = server.FindTopDocuments("fire water smoke"s);
        ASSERT_EQUAL(found_docs.size(), 2u);
        const Document& doc0 = found_docs[0];
        const Document& doc1 = found_docs[1];
        ASSERT_EQUAL(doc0.id, doc_id);
        ASSERT_EQUAL(doc1.id, doc_id1);
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        const auto found_docs = server.MatchDocument("-water smoke"s, doc_id);
        ASSERT_HINT(get<0>(found_docs).empty(), "Vector of matched words must be empty if doc contains minus word"s);
    }

}

void TestRelevanceDocumentsToQuery() {
    const int doc_id = 0;
    const string content = "smoke on the water"s;
    const vector<int> ratings = {1, 2, 3};
    const int doc_id1 = 1;
    const string content1 = "fire in the sky"s;
    const vector<int> ratings1 = {1, 2, 3};
    const int doc_id2 = 2;
    const string content2 = "lock stock two barrels"s;
    const vector<int> ratings2 = {1, 1, 1};

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock"s);
        ASSERT_EQUAL(found_docs.size(), 3u);
        ASSERT_HINT((found_docs[0].id == doc_id) && (found_docs[1].id == doc_id2) && (found_docs[2].id == doc_id1),
                    "Document with highest relevance must be first");
    }

}

void TestRatingOfDocumentIsAvrSum() {
    const int doc_id = 0;
    const string content = "smoke on the water"s;
    const vector<int> ratings = {0, 0, 0};
    const int doc_id1 = 1;
    const string content1 = "fire in the sky"s;
    const vector<int> ratings1 = {-10, 0, 10};
    const int doc_id2 = 2;
    const string content2 = "lock stock two barrels"s;
    const vector<int> ratings2 = {1, 1, 1};

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock"s);
        ASSERT_EQUAL(found_docs.size(), 3u);
        //cerr << found_docs[0].rating << " "s << found_docs[1].rating << " "s << found_docs[2].rating << endl;
        ASSERT_HINT((found_docs[0].rating == 1) && (found_docs[1].rating == 0) && (found_docs[2].rating == 0),
                    "Rating must be an average sum");
    }

}

void TestPredicateFilter() {
    const int doc_id = 0;
    const string content = "smoke on the water"s;
    const vector<int> ratings = {0, 0, 0};
    const int doc_id1 = 1;
    const string content1 = "fire in the sky"s;
    const vector<int> ratings1 = {-10, 0, 10};
    const int doc_id2 = 2;
    const string content2 = "lock stock two barrels"s;
    const vector<int> ratings2 = {1, 1, 1};

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock"s,
                        [](int document_id, DocumentStatus status, int rating) { return (document_id == 0); });
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_HINT((found_docs[0].id == 0), "Filter by id");
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock"s,
                        [](int document_id, DocumentStatus status, int rating) { return (status == DocumentStatus::BANNED); });
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_HINT((found_docs[0].id == 1), "Filter by status");
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock"s,
                        [](int document_id, DocumentStatus status, int rating) { return (rating > 0); });
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_HINT((found_docs[0].id == 2), "Filter by rating");
    }
    
}

void TestFilteringByStatus() {
    const int doc_id = 0;
    const string content = "smoke on the water"s;
    const vector<int> ratings = {0, 0, 0};
    const int doc_id1 = 1;
    const string content1 = "fire in the sky"s;
    const vector<int> ratings1 = {-10, 0, 10};
    const int doc_id2 = 2;
    const string content2 = "lock stock two barrels"s;
    const vector<int> ratings2 = {1, 1, 1};
    const int doc_id3 = 3;
    const string content3 = "from dusk till dawn"s;
    const vector<int> ratings3 = {1, 1, 1};

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock till dawn"s, DocumentStatus::ACTUAL);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_HINT((found_docs[0].id == 0), "Filter by ACTUAL");
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock till dawn"s, DocumentStatus::BANNED);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_HINT((found_docs[0].id == 1), "Filter by BANNED");
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock till dawn"s, DocumentStatus::IRRELEVANT);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_HINT((found_docs[0].id == 2), "Filter by IRRELEVANT");
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock till dawn"s, DocumentStatus::REMOVED);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_HINT((found_docs[0].id == 3), "Filter by REMOVED");
    }
    
}

void TestCorrectRelevanceCount() {
    const int doc_id = 0;
    const string content = "smoke on the water"s;
    const vector<int> ratings = {1, 2, 3};
    const int doc_id1 = 1;
    const string content1 = "fire in the sky"s;
    const vector<int> ratings1 = {1, 2, 3};
    const int doc_id2 = 2;
    const string content2 = "lock stock two barrels"s;
    const vector<int> ratings2 = {1, 1, 1};

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("fire water smoke lock stock barrels"s);
        ASSERT_EQUAL(found_docs.size(), 3u);
        //cerr << found_docs[0].relevance << " "s << found_docs[1].relevance << " "s << found_docs[2].relevance << endl;
        ASSERT_HINT((found_docs[0].relevance == (log(3.0 / 1.0) * 0.75)) && (found_docs[1].relevance == (log(3.0 / 1.0) * 0.5))
                    && (found_docs[2].relevance == (log(3.0 / 1.0) * 0.25)), "Query relevance is TF-IDF");
    }

}

void TestActualStatusFromMatchedDocuments() {
    const int doc_id = 0;
    const string content = "smoke on the water"s;
    const vector<int> ratings = {0, 0, 0};
    const int doc_id1 = 1;
    const string content1 = "fire in the sky"s;
    const vector<int> ratings1 = {-10, 0, 10};
    const int doc_id2 = 2;
    const string content2 = "lock stock two barrels"s;
    const vector<int> ratings2 = {1, 1, 1};
    const int doc_id3 = 3;
    const string content3 = "from dusk till dawn"s;
    const vector<int> ratings3 = {1, 1, 1};

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3);
        const auto found_docs = server.MatchDocument("fire water smoke lock stock till dawn"s, doc_id);
        ASSERT_HINT(get<1>(found_docs) == DocumentStatus::ACTUAL, "Query must have ACTUAL status"s);
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3);
        const auto found_docs = server.MatchDocument("fire water smoke lock stock till dawn"s, doc_id1);
        ASSERT_HINT(get<1>(found_docs) == DocumentStatus::BANNED, "Query must have BANNED status"s);
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3);
        const auto found_docs = server.MatchDocument("fire water smoke lock stock till dawn"s, doc_id2);
        ASSERT_HINT(get<1>(found_docs) == DocumentStatus::IRRELEVANT, "Query must have IRRELEVANT status"s);
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::BANNED, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3);
        const auto found_docs = server.MatchDocument("fire water smoke lock stock till dawn"s, doc_id3);
        ASSERT_HINT(get<1>(found_docs) == DocumentStatus::REMOVED, "Query must have REMOVED status"s);
    }
    
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    // Не забудьте вызывать остальные тесты здесь
    RUN_TEST(TestAddingDocsToAddDocumentFunction);
    RUN_TEST(TestExcludeMinusWordsFromResults);
    RUN_TEST(TestComplianceDocumentsToQuery);
    RUN_TEST(TestRelevanceDocumentsToQuery);
    RUN_TEST(TestRatingOfDocumentIsAvrSum);
    RUN_TEST(TestPredicateFilter);
    RUN_TEST(TestFilteringByStatus);
    RUN_TEST(TestCorrectRelevanceCount);
    RUN_TEST(TestActualStatusFromMatchedDocuments);
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
    string i;
    cin >> i;
}