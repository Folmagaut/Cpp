#pragma once

struct DayState {
public:
    double ComputeIncome() const {
        return earned - spent;
    }

    DayState& operator+=(const DayState& other) {
        earned += other.earned;
        spent += other.spent;
        return *this;
    }

    DayState operator+(const DayState& other) const {
        return DayState(*this) += other;
    }

    DayState operator*(double factor) const {
        return {earned * factor, spent * factor};
    }

public:
    double earned = 0.;
    double spent = 0.;
};
