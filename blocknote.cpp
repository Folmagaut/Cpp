#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include "log_duration.h"

using namespace std;

struct Person {
    string name;
    int age, income;
    bool is_male;
};

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;

    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = gender == 'M';
    }
    return result;
}

string MostPopularName(const map<string, size_t>& names) {
    pair<string, size_t> most_popular_name = {""s, 0};
    for (const auto& name : names) {
        if (name.second > most_popular_name.second) {
            most_popular_name = name;
        }
    }
    return most_popular_name.first;
}

int main() {
    vector<Person> people = ReadPeople(cin);

{
    LOG_DURATION("Time = "s);
    map<string, size_t> male_names;
    map<string, size_t> fem_names;
    for (const auto& person : people) {
        if (person.is_male) {
            male_names[person.name] += 1;
        } else {
            fem_names[person.name] += 1;
        }
    }

    vector<size_t> wealth(people.size());
    size_t wealth_counter = 0;
    size_t income = 0;
    sort(people.begin(), people.end(), 
                [](const Person& lhs, const Person& rhs) {
                return lhs.income > rhs.income;
                });
    for (const auto& person : people) {
        income += person.income;
        wealth[wealth_counter] = income;
        ++wealth_counter;
    }

    sort(people.begin(), people.end(), 
                [](const Person& lhs, const Person& rhs) {
                return lhs.age < rhs.age;
                });

    for (string command; cin >> command;) {
        
        if (command == "AGE"s) {
            int adult_age;
            cin >> adult_age;

            auto adult_begin = lower_bound(people.begin(), people.end(), adult_age, 
            	[](const Person& lhs, int age) {
                return lhs.age < age;
            });

            cout << "There are "s << distance(adult_begin, people.end()) << " adult people for maturity age "s
                 << adult_age << '\n';
        } else if (command == "WEALTHY"s) {
            int count;
            cin >> count;
            int total_income = 0;
            if (count > 0) {
                total_income = wealth[min(count - 1, static_cast<int>(people.size() - 1))];
            }
            cout << "Top-"s << count << " people have total income "s << total_income << '\n';
        } else if (command == "POPULAR_NAME"s) {
            char gender;
            cin >> gender;
            string most_popular_name;
            if (gender == 'M') {
                most_popular_name = MostPopularName(male_names);
            } else {
                most_popular_name = MostPopularName(fem_names);
            }
            cout << "Most popular name among people of gender "s << gender << " is "s << most_popular_name << '\n';
        }
    }
}
}
