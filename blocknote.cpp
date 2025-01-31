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
        // Разделяем домен на части
        size_t start = 0;
        size_t pos;
        while ((pos = domain.find('.', start)) != string::npos) {
            parts_.emplace_back(domain.substr(start, pos - start));
            start = pos + 1;
        }
        parts_.emplace_back(domain.substr(start));
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
        if (parts_.size() > other.parts_.size()) {
            return false;
        }
        for (size_t i = 0; i < parts_.size(); ++i) {
            if (parts_[parts_.size() - 1 - i] != other.parts_[other.parts_.size() - 1 - i]) {
                return false;
            }
        }
        return true;
    }

    string GetDomain() const {
        return domain_;
    }

private:
    string domain_;
    vector<string> parts_;
};

class DomainChecker {
public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов
    DomainChecker(vector<Domain>::const_iterator begin, vector<Domain>::const_iterator end) {
        for (auto it = begin; it != end; ++it) {
            //forbidden_domains_.insert(it->GetDomain());
            f_d_.insert(Domain(it->GetDomain()));
        }
    }
    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& domain) const {
        auto it = f_d_.lower_bound(domain);
        return it != f_d_.end() && it->IsSubdomain(domain);
    }

private:
    //set<string> forbidden_domains_;
    set<Domain> f_d_;
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