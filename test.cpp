#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <set>

using namespace std;

class Domain {
public:
    Domain(const string& domain) : domain_(domain) {
        // Разделяем домен на части
        size_t start = 0;
        size_t pos;
        while ((pos = domain.find('.', start)) != string::npos) {
            parts_.push_back(domain.substr(start, pos - start));
            start = pos + 1;
        }
        parts_.push_back(domain.substr(start));
    }

    bool operator==(const Domain& other) const {
        return domain_ == other.domain_;
    }

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
    DomainChecker(vector<Domain>::const_iterator begin, vector<Domain>::const_iterator end) {
        for (auto it = begin; it != end; ++it) {
            forbidden_domains_.insert(it->GetDomain());
        }
    }

    bool IsForbidden(const Domain& domain) const {
        for (const string& forbidden_domain : forbidden_domains_) {
            Domain forbidden(forbidden_domain);
            if (domain.IsSubdomain(forbidden)) {
                return true;
            }
        }
        return false;
    }

private:
    set<string> forbidden_domains_;
};

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