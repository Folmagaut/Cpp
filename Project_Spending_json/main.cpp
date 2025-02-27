#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

// не меняйте файлы json.h и json.cpp
#include "json.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(
        spendings.begin(), spendings.end(), 0, [](int current, const Spending& spending){
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

vector<Spending> LoadFromJson(istream& input) {
    // место для вашей реализации
    // пример корректного JSON-документа в условии
    vector<Spending> js_vec;
    Document js_doc = Load(input);
    for (auto& doc : js_doc.GetRoot().AsArray()) {
        string category = doc.AsMap().at("category").AsString();
        int amount = doc.AsMap().at("amount").AsInt();
        js_vec.push_back({category, amount});
    }
    return js_vec;
}

int main() {
    // не меняйте main
    //ifstream file("file");
    //const vector<Spending> spendings = LoadFromJson(file);
    const vector<Spending> spendings = LoadFromJson(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}