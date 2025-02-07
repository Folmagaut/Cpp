#pragma once

#include "bulk_updater.h"
#include "date.h"
#include "entities.h"
#include "summing_segment_tree.h"

using namespace std::literals;

//static const Date START_DATE = Date("2000-01-01"s);
//static const Date END_DATE = Date("2100-01-01"s);

class BudgetManager {
public:
    static const Date START_DATE;
    static const Date END_DATE;

    static size_t GetDayIndex(const Date& day) {
        return static_cast<size_t>(Date::ComputeDistance(START_DATE, day));
    }

    static IndexSegment MakeDateSegment(const Date& from, const Date& to) {
        return {GetDayIndex(from), GetDayIndex(to) + 1};
    }

    DayBudget ComputeSum(const Date& from, const Date& to) const {
        return tree_.ComputeSum(MakeDateSegment(from, to));
    }

    void AddBulkOperation(const Date& from, const Date& to, const BulkLinearUpdater& operation) {
        tree_.AddBulkOperation(MakeDateSegment(from, to), operation);
    }

private:
    SummingSegmentTree<DayBudget, BulkLinearUpdater> tree_{GetDayIndex(END_DATE)};
};