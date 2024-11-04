#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

// не меняйте файлы xml.h и xml.cpp
#include "xml.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(spendings.begin(), spendings.end(), 0,
                      [](int current, const Spending& spending) {
                          return current + spending.amount;
                      });
}

string FindMostExpensiveCategory(const vector<Spending>& spendings) {
    assert(!spendings.empty());
    auto compare_by_amount = [](const Spending& lhs, const Spending& rhs) {
        return lhs.amount < rhs.amount;
    };
    return max_element(begin(spendings), end(spendings), compare_by_amount)->category;
}

vector<Spending> LoadFromXml(istream& input) {
    // место для вашей реализации
    // пример корректного XML-документа в условии
    vector<Spending> xml_vec;
    Document xml_doc = Load(input);
    for (auto& doc : xml_doc.GetRoot().Children()) {
        string category = doc.AttributeValue<string>("category");
        int amount = doc.AttributeValue<int>("amount");
        xml_vec.push_back({category, amount});
    }
    return xml_vec;
}

int main() {
    const vector<Spending> spendings = LoadFromXml(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}