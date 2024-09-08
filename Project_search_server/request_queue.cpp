#include "request_queue.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server) : requests_(), no_result_counter_(0), current_time_(0), search_server_(search_server) {
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    vector<Document> search_results = search_server_.FindTopDocuments(raw_query, status);
    AddRequest(search_results);
    return search_results;
}

vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    vector<Document> search_results = search_server_.FindTopDocuments(raw_query);
    AddRequest(search_results);
    return search_results;
}

int RequestQueue::GetNoResultRequests() const {
    return no_result_counter_;
}

void RequestQueue::AddRequest(vector<Document>& search_results) {
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