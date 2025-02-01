#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

class Domain {
public:
    // разработайте класс домена
    // конструктор должен позволять конструирование из string, с сигнатурой определитесь сами
    Domain(string_view domain) : domain_(domain) {
        reverse(domain_.begin(), domain_.end());
        domain_.push_back('.');
    }
    // разработайте operator==
    bool operator==(const Domain& other) const {
        return domain_ == other.domain_;
    }

    bool operator<(const Domain& other) const {
        return domain_ < other.domain_;
    }

    // разработайте метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
    bool IsSubdomain(const Domain& other) const {
        if (other.domain_.size() > domain_.size()) {
            return false;
        }
        auto contains_it = std::mismatch(other.domain_.begin(), other.domain_.end(), domain_.begin());
        if (contains_it.first != other.domain_.end()) {
            return false;
        }
        return true;
    }

    string GetDomain() const {
        return domain_;
    }

private:
    string domain_;
    //vector<string> parts_;
};

class DomainChecker {
public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов
    DomainChecker(vector<Domain>::const_iterator begin, vector<Domain>::const_iterator end) : forbidden_domains_(begin, end) {
        sort(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& lhs, const Domain& rhs) {
            return lhs.GetDomain() < rhs.GetDomain();
        });
        end = std::unique(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& lhs, const Domain& rhs) {
            return rhs.IsSubdomain(lhs);
        });
        forbidden_domains_.erase(end, forbidden_domains_.end());
    }
    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& domain) const {
        auto it = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domain, [](const Domain& lhs, const Domain& rhs){
                    return lhs.GetDomain() < rhs.GetDomain();
                });
        if (it == forbidden_domains_.begin()) {
            return false;
        } else {
            return domain.IsSubdomain(*prev(it));
        }
    }

private:
    //set<string> forbidden_domains_;
    vector<Domain> forbidden_domains_;
};

// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа
vector<Domain> ReadDomains(istream& input, size_t count) {
    vector<Domain> domains;
    for (size_t i = 0; i < count; ++i) {
        string domain;
        getline(input, domain);
        domains.emplace_back(domain);
    }
    return domains;
}

template <typename Number>
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