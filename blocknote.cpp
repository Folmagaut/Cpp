#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double EPSILON = 1e-6;

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

struct Document {
    Document() = default;
    Document(int id, double relevance, int rating)
        : id(id)
        , relevance(relevance)
        , rating(rating) {
    }
    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

template <typename Iterator>
class Paginator {
public:
    
    Paginator(Iterator first, Iterator last, size_t page_size) : m_page_size_(page_size), vec_pages_(MakeVec(first, last)) {
    }

    vector<pair<Iterator, Iterator>> MakeVec(Iterator begin, Iterator end) {
        vector<pair<Iterator, Iterator>> vec;
        Iterator iter = begin;
        if (distance(begin, end) <= m_page_size_) {
            for (auto it = begin; it != end + 1; ++ it) {
                vec.push_back(make_pair(begin, end));
            }
            return vec;
        } else {
            for (auto it = begin; it != end + 1; ++it) {
                if (it < (iter + m_page_size_)) {
                    if (it == end) {
                        vec.push_back(make_pair(iter, end - 1));
                        return vec;
                    }
                    continue;
                } else {
                    vec.push_back(make_pair(iter, iter + m_page_size_ - 1));
                    advance(iter, m_page_size_);
                }
            }
            return vec;
        }
    }

    auto begin() const {
        return vec_pages_.begin();
    }

    auto end() const {
        return vec_pages_.end();
    }

private:
    size_t m_page_size_;
    vector<pair<Iterator, Iterator>> vec_pages_;
};

ostream& operator<<(ostream& output, const Document& document) {
    return output << "{ document_id = "s << document.id << ", relevance = "s << document.relevance << ", rating = "s << document.rating << " }"s;
}
    
template <typename Iterator>
ostream& operator<<(ostream& output, const pair<Iterator, Iterator>& single_page_vec) {
            for (auto it = single_page_vec.first; it != single_page_vec.second + 1; ++it) {
                output << *it;
            }        
    return output;
}

template <typename StringContainer>
set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    set<string> non_empty_strings;
    for (const string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words) : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
        if (!all_of(stop_words.begin(), stop_words.end(), IsValidWord)) {
            throw invalid_argument("Invalid_argument");
        }
    }

    explicit SearchServer(const string& stop_words_text) : SearchServer(SplitIntoWords(stop_words_text)) {
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
   
        if (document_id < 0) {
            throw invalid_argument("Invalid_argument");
        }
        if (docs_id_in_a_row_.size() != 0) {
            if (count(docs_id_in_a_row_.begin(), docs_id_in_a_row_.end(), document_id)) {
                throw invalid_argument("Invalid_argument");
            }
        }
        const vector<string> words = SplitIntoWordsNoStop(document);

        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
        docs_id_in_a_row_.push_back(document_id);
    }

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {
        const Query query = ParseQuery(raw_query);

        auto matched_documents = FindAllDocuments(query, document_predicate);
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 if (abs(lhs.relevance - rhs.relevance) < EPSILON) {
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

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(
            raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
                return document_status == status;
            });
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
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

    int GetDocumentId(int index) const {
        return docs_id_in_a_row_.at(index);
    }

private:
    vector<int> docs_id_in_a_row_;

    static bool IsValidWord(const string& word) {
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
        });
    }

    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    const set<string> stop_words_;
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
        if (!IsValidWord(text)) {
            throw invalid_argument("Invalid_argument");
        }
        if (text[0] == '-') {
            text = text.substr(1);
            if (text[0] == '-' || text.empty()) {
                throw invalid_argument("Invalid_argument");
            }
            is_minus = true;
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

    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query,
                                      DocumentPredicate document_predicate) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto &[document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance) {
            matched_documents.push_back(
                {document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }
};

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server) : requests_(), no_result_counter_(0), current_time_(0), search_server_(search_server) {
    }

    template <typename DocumentPredicate>
    void AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
        vector<Document> search_results = search_server_.FindTopDocuments(raw_query, document_predicate);
        AddRequest(search_results);
    }

    void AddFindRequest(const string& raw_query, DocumentStatus status) {
        vector<Document> search_results = search_server_.FindTopDocuments(raw_query, status);
        AddRequest(search_results);
    }

    void AddFindRequest(const string& raw_query) {
        vector<Document> search_results = search_server_.FindTopDocuments(raw_query);
        AddRequest(search_results);
    }

    int GetNoResultRequests() const {
        return no_result_counter_;
    }

private:
    struct QueryResult {
        bool is_empty;
        int query_time;
    };
    deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int no_result_counter_;
    int current_time_;
    const SearchServer& search_server_;

    void AddRequest(vector<Document>& search_results) {
        QueryResult result;
        ++current_time_;
        if (search_results.size() == 0) {
            result.is_empty = true;
            result.query_time = current_time_ % min_in_day_;
            ++no_result_counter_;
        } else {
            result.is_empty = false;
            result.query_time = current_time_ % min_in_day_;
        }
        requests_.push_back(result);
        if (requests_.size() - 1 == min_in_day_) {
            QueryResult front_result = *requests_.begin();
            if (front_result.is_empty) {
                --no_result_counter_;
            }
            requests_.pop_front();
        }
    }
};

// ==================== для примера =========================

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

int main() {
    SearchServer search_server("and in at"s);
    RequestQueue request_queue(search_server);
    search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, {7, 2, 7});
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, {1, 2, 3});
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, {1, 2, 8});
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, {1, 3, 2});
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, {1, 1, 1});
    // 1439 запросов с нулевым результатом
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request"s);
    }
    // все еще 1439 запросов с нулевым результатом
    request_queue.AddFindRequest("curly dog"s);
    // новые сутки, первый запрос удален, 1438 запросов с нулевым результатом
    request_queue.AddFindRequest("big collar"s);
    // первый запрос удален, 1437 запросов с нулевым результатом
    request_queue.AddFindRequest("sparrow"s);
    cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
    return 0;
}