#pragma once
#include <cstdint>
#include <ostream>
#include <vector>

template <typename Iterator>
class Paginator {
public:
    
    Paginator(Iterator first, Iterator last, size_t page_size) : m_page_size_(page_size), vec_pages_(MakeVec(first, last)) {
    }

    std::vector<std::pair<Iterator, Iterator>> MakeVec(Iterator begin, Iterator end) {
        std::vector<std::pair<Iterator, Iterator>> vec;
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
    std::vector<std::pair<Iterator, Iterator>> vec_pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

template <typename Iterator>
std::ostream& operator<<(std::ostream& output, const std::pair<Iterator, Iterator>& single_page_vec) {
            for (auto it = single_page_vec.first; it != single_page_vec.second + 1; ++it) {
                output << *it;
            }        
    return output;
}