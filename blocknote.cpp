#include <iostream>
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

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

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

int main() {
    vector<Document> search_results;
    
    for (int i = 0; i < 8; ++i) {
        Document doc(i, i, i);
        search_results.push_back(doc);
    }
    size_t page_size = 3;
    //cout << search_results.size() << endl;
    const auto pages = Paginate(search_results, page_size);
    // Выводим найденные документы по страницам
    for (auto page = pages.begin(); page != pages.end(); ++page) {
        cout << *page << endl;
        cout << "Page break"s << endl;
    }
}