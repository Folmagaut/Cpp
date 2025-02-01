#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>


using namespace std;

class Domain {
public:
    Domain(string_view src) {
        repr_ = string(src.rbegin(), src.rend()) + "."s;
    }

    bool IsSubdomain(const Domain& r) const {
        return repr_.size() >= r.repr_.size() && repr_.substr(0, r.repr_.size()) == r.repr_;
    }

    bool operator==(const Domain& r) const {
        return repr_ == r.repr_;
    }

    bool operator<(const Domain& r) const {
        // сравниваем лексикографически, считая точку самым маленьким символом
        return std::lexicographical_compare(repr_.begin(), repr_.end(), r.repr_.begin(), r.repr_.end(),
                                            [](char l, char r) {
                                                return (r != '.') && (l < r || l == '.');
                                            });
    }

private:
    // repr_ содержит название домена, записанное задом наперёд, с приписанным в конец символом точки
    string repr_;
};

class DomainChecker {
public:
    template <typename InputIt>
    DomainChecker(InputIt begin, InputIt end) {
        std::vector<Domain> all_domains(begin, end);
        sort(all_domains.begin(), all_domains.end());
        sorted_domains_ = AbsorbSubdomains(std::move(all_domains));
    }

    bool IsForbidden(const Domain& domain) const {
        auto iter = upper_bound(sorted_domains_.begin(), sorted_domains_.end(), domain);
        if (iter == sorted_domains_.begin()) {
            return false;
        }

        return domain.IsSubdomain(*prev(iter));
    }

private:
    static vector<Domain> AbsorbSubdomains(vector<Domain> domains) {
        domains.erase(unique(begin(domains), end(domains),
                             [](const Domain& lhs, const Domain& rhs) {
                                 return lhs.IsSubdomain(rhs) || rhs.IsSubdomain(lhs);
                             }),
                      end(domains));
        return domains;
    }

private:
    std::vector<Domain> sorted_domains_;
};

std::vector<Domain> ReadDomains(istream& input, size_t n) {
    std::vector<Domain> result_vector;
    result_vector.reserve(n);

    for (size_t i = 0; i < n; ++i) {
        string domain;
        getline(input, domain);

        result_vector.push_back(Domain(domain));
    }

    return result_vector;
}

template<typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}