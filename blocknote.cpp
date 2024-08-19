#include <iostream>
#include <numeric>
#include <string>

using namespace std;

class Rational {
public:
    Rational() = default;

    Rational(int value)
        : numerator_(value)
        , denominator_(1)
    {
    }

    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator)
    {
        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

    Rational& operator+=(Rational right) {
        numerator_ = numerator_ * right.denominator_ + right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator-=(Rational right) {
        numerator_ = numerator_ * right.denominator_ - right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator*=(Rational right) {
        numerator_ *= right.numerator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator/=(Rational right) {
        numerator_ *= right.denominator_;
        denominator_ *= right.numerator_;
        Normalize();
        return *this;
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        int n = gcd(numerator_, denominator_);
        numerator_ /= n;
        denominator_ /= n;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

ostream& operator<<(ostream& output, Rational rational) {
    return output << rational.Numerator() << '/' << rational.Denominator();
}

istream& operator>>(istream& input, Rational& rational) {
    int numerator;
    int denominator;
    char slash;
    if ((input >> numerator) && (input >> slash) && (slash == '/') && (input >> denominator)) {
        rational = Rational{numerator, denominator};
    }
    return input;
}

Rational operator+(Rational left, Rational right) {
    left += right;
    return left;
    //const int numerator = left.Numerator() * right.Denominator() + right.Numerator() * left.Denominator();
    //const int denominator = left.Denominator() * right.Denominator();
    //return {numerator, denominator};
}

Rational operator-(Rational left, Rational right) {
    left -= right;
    return left;
    //const int numerator = left.Numerator() * right.Denominator() - right.Numerator() * left.Denominator();
    //const int denominator = left.Denominator() * right.Denominator();
    //return {numerator, denominator};
}

Rational operator+(Rational value) {
    return value;
}

Rational operator-(Rational value) {
    return {-value.Numerator(), value.Denominator()};
}

Rational operator*(Rational left, const Rational right) {
    left *= right;
    return left;
    //const int numerator = left.Numerator() * right.Numerator();
    //const int denominator = left.Denominator() * right.Denominator();
    //return {numerator, denominator};
    }

Rational operator/(Rational left, const Rational right) {
    left /= right;
    return left;
    //const int numerator = left.Numerator() * right.Denominator();
    //const int denominator = left.Denominator() * right.Numerator();
    //return {numerator, denominator};
    } 

bool operator==(Rational left, Rational right) {
    return left.Numerator() == right.Numerator() && 
           left.Denominator() == right.Denominator();
}

bool operator!=(Rational left, Rational right) {
    return !(left == right);
}

bool operator<(const Rational left, const Rational right) {
    return left.Numerator() * right.Denominator() < right.Numerator() * left.Denominator();
}

bool operator>(const Rational left, const Rational right) {
    return left.Numerator() * right.Denominator() > right.Numerator() * left.Denominator();
}

bool operator<=(const Rational left, const Rational right) {
    return !(left > right);
}

bool operator>=(const Rational left, const Rational right) {
    return !(left < right);
}

/* Rational& operator+=(Rational& left, const Rational& right) {
    left = left + right;
    return left;
}

Rational& operator-=(Rational& left, const Rational& right) {
    left = left - right;
    return left;
}

Rational& operator*=(Rational& left, const Rational& right) {
    left = left * right;
    return left;
}

Rational& operator/=(Rational& left, const Rational& right) {
    left = left / right;
    return left;
}
 */
int main() {
    Rational scale1; // (-6, -24)
    Rational scale2;
    cin >> scale1 >> scale2;
    scale1 += scale2;
    cout << scale1 << endl;
}