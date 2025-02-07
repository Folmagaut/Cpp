#include "parser.h"
#include "bulk_updater.h"
#include "entities.h"

#include <unordered_map>

namespace queries {

class ComputeIncome : public ComputeQuery {
public:
    using ComputeQuery::ComputeQuery;

    [[nodiscard]] ReadResult Process(const BudgetManager &budget) const override {
        return {budget.ComputeSum(GetFrom(), GetTo())};
    }

    class Factory : public QueryFactory {
    public:
        [[nodiscard]] std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            return std::make_unique<ComputeIncome>(Date(parts[0]), Date(parts[1]));
        }
    };
};

class Alter : public ModifyQuery {
public:
    Alter(Date from, Date to, double amount)
        : ModifyQuery(from, to), amount_(amount) {
    }

    void Process(BudgetManager& budget) const override {
        double day_income = amount_ / (Date::ComputeDistance(GetFrom(), GetTo()) + 1);
        const DayState daily_change = day_income > 0 ? DayState{day_income, 0.} : DayState{0., -day_income};
        budget.AddBulkOperation(GetFrom(), GetTo(), BulkMoneyAdder{daily_change});
    }

    class FactoryEarn : public QueryFactory {
    public:
        [[nodiscard]] std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            double payload = std::stod(std::string(parts[2]));
            return std::make_unique<Alter>(Date(parts[0]), Date(parts[1]), payload);
        }
    };

    class FactorySpend : public QueryFactory {
    public:
        [[nodiscard]] std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            double payload = std::stod(std::string(parts[2]));
            return std::make_unique<Alter>(Date(parts[0]), Date(parts[1]), -payload);
        }
    };

private:
    double amount_;
};

class PayTax : public ModifyQuery {
public:
    using ModifyQuery::ModifyQuery;

    PayTax(Date from, Date to, double rate)
        : ModifyQuery(from, to), rate_(rate) {
    }

    void Process(BudgetManager& budget) const override {
        budget.AddBulkOperation(GetFrom(), GetTo(), BulkTaxApplier{1. - rate_});
    }

    class Factory : public QueryFactory {
    public:
        [[nodiscard]] std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            int payload = std::stoi(std::string(parts[2]));
            return std::make_unique<PayTax>(Date(parts[0]), Date(parts[1]), payload / 100.);
        }
    };

private:
    double rate_;
};

}  // namespace queries

const QueryFactory& QueryFactory::GetFactory(std::string_view id) {
    using namespace std::literals;

    static queries::ComputeIncome::Factory compute_income;
    static queries::Alter::FactoryEarn earn;
    static queries::Alter::FactorySpend spend;
    static queries::PayTax::Factory pay_tax;
    static std::unordered_map<std::string_view, const QueryFactory &> factories
            = {{"ComputeIncome"sv, compute_income},
               {"Earn"sv,          earn},
               {"Spend"sv,         spend},
               {"PayTax"sv,        pay_tax}};

    return factories.at(id);
}
