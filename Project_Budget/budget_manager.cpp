#include <iostream>
#include "budget_manager.h"

// разработайте менеджер личного бюджета самостоятельно

using namespace std::literals;

const Date BudgetManager::START_DATE = Date("2000-01-01"s);
const Date BudgetManager::END_DATE = Date("2100-01-01"s);

void BudgetManager::ComputeIncome(const Date from, const Date to) const {
    double result = 0.0;
    const int start = from.ComputeDistance(START_DATE, from);
    const int end = to.ComputeDistance(START_DATE, to);
    for (int i = start; i <= end; ++i) {
        result += all_days_[i].earnings - all_days_[i].spents;
    }
    std::cout << result << std::endl;
}

void BudgetManager::Earn(const Date from, const Date to, double earnings) {
    const int start = from.ComputeDistance(START_DATE, from);
    const int end = to.ComputeDistance(START_DATE, to);
    int period = end - start + 1;
    double ear_per_day = earnings / period;
    for (int i = start; i <= end; ++i) {
        all_days_[i].earnings += ear_per_day;
    }
}

void BudgetManager::PayTax(const Date from, const Date to, double tax) {
    const int start = from.ComputeDistance(START_DATE, from);
    const int end = to.ComputeDistance(START_DATE, to);
    double rest = (100 - tax) / 100;
    for (int i = start; i <= end; ++i) {
        all_days_[i].earnings *= rest;
    }
}

void BudgetManager::Spend(const Date from, const Date to, double spents) {
    const int start = from.ComputeDistance(START_DATE, from);
    const int end = to.ComputeDistance(START_DATE, to);
    int period = end - start + 1;
    double spend_per_day = spents / period;
    for (int i = start; i <= end; ++i) {
        all_days_[i].spents += spend_per_day;
    }
}