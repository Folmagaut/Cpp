#pragma once

#include "entities.h"
#include "summing_segment_tree.h"

struct BulkMoneyAdder {
    DayState delta = {};
};

struct BulkTaxApplier {
    double factor = 1.0;
};

class BulkLinearUpdater {
public:
    BulkLinearUpdater() = default;

    BulkLinearUpdater(const BulkMoneyAdder& add)
        : add_(add) {
    }

    BulkLinearUpdater(const BulkTaxApplier& tax)
        : tax_(tax) {
    }

    void CombineWith(const BulkLinearUpdater& other) {
        tax_.factor *= other.tax_.factor;
        add_.delta.spent += other.add_.delta.spent;
        add_.delta.earned = add_.delta.earned * other.tax_.factor + other.add_.delta.earned;
    }

    DayState Collapse(const DayState& origin, IndexSegment segment) const {
        return DayState{origin.earned * tax_.factor, origin.spent}
               + add_.delta * static_cast<double>(segment.length());
    }

private:
    // Apply tax first, then add.
    BulkTaxApplier tax_;
    BulkMoneyAdder add_;
};
