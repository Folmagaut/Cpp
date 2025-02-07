#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "budget_manager.h"
#include "date.h"

struct ReadResult {
    void Print(std::ostream& out) const {
        out << total.income - total.spend << std::endl;
    }

    DayBudget total;
};

inline std::pair<std::string_view, std::optional<std::string_view>> SplitFirst(std::string_view input, char c) {
    auto cpos = input.find(c);
    if (cpos == std::string_view::npos) {
        return {input, {}};
    }

    return {input.substr(0, cpos), input.substr(cpos + 1)};
}

inline std::vector<std::string_view> Split(std::string_view input, char c) {
    std::vector<std::string_view> res;
    while (true) {
        auto [l, r] = SplitFirst(input, c);
        res.push_back(l);
        if (!r) {
            return res;
        }
        input = *r;
    }
}

class Query {
public:
    Query(Date from, Date to)
        : from_(from), to_(to) {
    }

    [[nodiscard]] Date GetFrom() const {
        return from_;
    }

    [[nodiscard]] Date GetTo() const {
        return to_;
    }

    virtual void ProcessAndPrint(BudgetManager& budget, std::ostream& out) const = 0;

    virtual ~Query() = default;

private:
    Date from_, to_;
};

class ComputeQuery : public Query {
public:
    using Query::Query;
    virtual ~ComputeQuery() = default;
    [[nodiscard]] virtual ReadResult Process(const BudgetManager& budget) const = 0;

    void ProcessAndPrint(BudgetManager& budget, std::ostream& out) const override {
        Process(budget).Print(out);
    }

    //~ComputeQuery() override = default;
};

class ModifyQuery : public Query {
public:
    using Query::Query;

    virtual void Process(BudgetManager& budget) const = 0;

    void ProcessAndPrint(BudgetManager& budget, std::ostream&) const override {
        Process(budget);
    }
    virtual ~ModifyQuery() = default;
    //~ModifyQuery() override = default;
};

class QueryFactory {
public:
    [[nodiscard]] virtual std::unique_ptr<Query> Construct(std::string_view config) const = 0;

    static const QueryFactory& GetFactory(std::string_view id);

    virtual ~QueryFactory() = default;
};

inline std::unique_ptr<Query> ParseQuery(std::string_view line) {
    auto [command, pconfig] = SplitFirst(line, ' ');

    const auto& factory = QueryFactory::GetFactory(command);
    return factory.Construct(pconfig.value_or(std::string_view{}));
}

inline std::vector<std::unique_ptr<Query>> ReadQueries(std::istream& input) {
    std::vector<std::unique_ptr<Query>> result;
    std::string line;
    std::getline(input, line);
    const int query_count = std::stoi(line);
    result.reserve(query_count);

    for (int i = 0; i < query_count; ++i) {
        std::getline(input, line);
        result.push_back(ParseQuery(line));
    }

    return result;
}