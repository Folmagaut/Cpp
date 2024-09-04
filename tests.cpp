template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
        : first_(begin)
        , last_(end)
        , size_(distance(first_, last_)) {
    }

    Iterator begin() const {
        return first_;
    }

    Iterator end() const {
        return last_;
    }

    size_t size() const {
        return size_;
    }

private:
    Iterator first_, last_;
    size_t size_;
};

template <typename Iterator>
ostream& operator<<(ostream& out, const IteratorRange<Iterator>& range) {
    for (Iterator it = range.begin(); it != range.end(); ++it) {
        out << *it;
    }
    return out;
}

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t page_size) {
        for (size_t left = distance(begin, end); left > 0;) {
            const size_t current_page_size = min(page_size, left);
            const Iterator current_page_end = next(begin, current_page_size);
            pages_.push_back({begin, current_page_end});

            left -= current_page_size;
            begin = current_page_end;
        }
    }

    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }

    size_t size() const {
        return pages_.size();
    }

private:
    vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
////////////////////////////////////////
/*
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
 
using namespace std;
 
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
    Paginator(Iterator first, Iterator last, size_t page_size) {
        while (first != last) {
            Iterator current_end = first;
            advance(current_end, min(page_size, static_cast<size_t>(distance(first, last))));
            pages_.push_back(make_pair(first, current_end));
            first = current_end;
        }
    }
 
    auto begin() const {
        return pages_.begin();
    }
 
    auto end() const {
        return pages_.end();
    }
 
    size_t size() const {
        return pages_.size();
    }
 
private:
    vector<pair<Iterator, Iterator>> pages_;
};
 
template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
 
ostream& operator<<(ostream& output, const Document& document) {
    return output << "{ document_id = "s << document.id
                  << ", relevance = "s << document.relevance
                  << ", rating = "s << document.rating << " }"s;
}
 
template <typename Iterator>
ostream& operator<<(ostream& output, const pair<Iterator, Iterator>& page) {
    for (auto it = page.first; it != page.second; ++it) {
        output << *it;
    }
    return output;
}
 
int main() {
    vector<Document> search_results;
 
    for (int i = 0; i < 3; ++i) {
        Document doc(i, i, i);
        search_results.push_back(doc);
    }
    int page_size = 2;
    const auto pages = Paginate(search_results, page_size);
 
    // Выводим найденные документы по страницам
    for (const auto& page : pages) {
        cout << page << endl;
        cout << "Page break"s << endl;
    }
}
*/

/* #include <iostream>
#include <iterator>
#include <vector>
#include <string>

using namespace std;

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
    //Paginator() = default;
    
    Paginator(Iterator first, Iterator last, size_t page_size) : m_page_size_(page_size), vec_pages_(MakeVec(first, last)) {
    }
    vector<vector<Iterator>> MakeVec(Iterator begin, Iterator end) {
        vector<vector<Iterator>> vec;
        Iterator iter = begin;
        auto dist = distance(begin, end);
        if (distance(begin, end) <= m_page_size_) {
            vector<Iterator> single_page_vec = {};
            for (auto it = begin; it != end; ++it) {
                single_page_vec.push_back(it);
            }
            vec.push_back(single_page_vec);
            return vec;
        } else {
            vector<Iterator> single_page_vec = {};
            for (auto it = begin; it != end; ++it) {
                //vector<Iterator> single_page_vec = {};
                if (it < (iter + m_page_size_)) {
                    single_page_vec.push_back(it);
                    if (it == end - 1) {
                        vec.push_back(single_page_vec);
                        return vec;
                    }
                    continue;
                } else {
                    vec.push_back(single_page_vec);
                    single_page_vec = {};
                    advance(iter, m_page_size_);
                }
            }
            //vec_pages_ = vec;
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
    //pair<Iterator, Iterator> m_pages_;
    vector<vector<Iterator>> vec_pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

ostream& operator<<(ostream& output, const Document& document) {
    return output << "{ document_id = "s << document.id << ", relevance = "s << document.relevance << ", rating = "s << document.rating << " }"s;
}

template <typename Iterator>
ostream& operator<<(ostream& output, const Paginator<vector<vector<Iterator>>>& paginator) {
    
    for (size_t i = 0; i < paginator.vec_pages_.size(); ++i) {
        for (const auto page : paginator.vec_pages_[i]) {
            //for (auto it = paginator.vec_pages_[i].begin(); it != paginator.vec_pages_[i].end(); ++it) {
                output << *page;
            //}
        }
    }
        
    return output;
}

template <typename Iterator>
ostream& operator<<(ostream& output, const vector<vector<Iterator>>& vec_pages) {
    
    for (size_t i = 0; i < vec_pages.size(); ++i) {
        for (const auto page : vec_pages[i]) {
            //for (auto it = paginator.vec_pages_[i].begin(); it != paginator.vec_pages_[i].end(); ++it) {
                output << *page;
            //}
        }
    }
        
    return output;
}

int main() {
    vector<Document> search_results;
    
    for (int i = 0; i < 5; ++i) {
        Document doc(i, i, i);
        search_results.push_back(doc);
    }
    size_t page_size = 2;
    //cout << search_results.size() << endl;
    const auto pages = Paginate(search_results, page_size);
    // Выводим найденные документы по страницам
    for (auto page = pages.begin(); page != pages.end(); ++page) {
        cout << *page << endl;
        cout << "Page break"s << endl;
    }
} */