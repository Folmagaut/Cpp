#pragma once
#include "date.h"

//inline static const double TAX = 0.87;

struct DayInfo {
    double earnings = 0.0;
    double spents  = 0.0;
};

class BudgetManager {
public:
    static const Date START_DATE;
    static const Date END_DATE;

    // разработайте класс BudgetManager
    BudgetManager() : all_days_(START_DATE.ComputeDistance(START_DATE, END_DATE), DayInfo()) {
    }

    void ComputeIncome(const Date from, const Date to) const;
    void Earn(const Date from, const Date to, double earnings);
    void PayTax(const Date from, const Date to, double tax);
    void Spend(const Date from, const Date to, double spents);

private:
    std::vector<DayInfo> all_days_;
};