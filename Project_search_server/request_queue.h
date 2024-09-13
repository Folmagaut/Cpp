#pragma once
#include <deque>
#include <vector>
#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    // исправлено
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        bool is_empty;
        int query_time;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int no_result_counter_;
    int current_time_;
    const SearchServer& search_server_;

    void AddRequest(std::vector<Document>& search_results);
};

template <typename DocumentPredicate>
    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        std::vector<Document> search_results = search_server_.FindTopDocuments(raw_query, document_predicate);
        AddRequest(search_results);
        return search_results;
    }