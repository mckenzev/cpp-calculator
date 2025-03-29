#pragma once

#include <compare>
#include <cstdint>
#include <iostream>
#include <numeric>

class Rational {
public:
    Rational() = default;
    Rational(int numerator) : numerator_(numerator) {}
    Rational(int numerator, int denominator)
     : numerator_{numerator}, denominator_{denominator} {
        Reduction();
     }
    Rational(const Rational& other) = default;

    Rational operator+() const {
        return *this;
    }

    Rational operator-() const {
        return {-numerator_, denominator_};
    }

    Rational operator+(const Rational& other) const {
        Rational result = *this;
        result += other;
        return result;
    }

    Rational operator-(const Rational& other) const {
        Rational result = *this;
        result -= other;
        return result;
    }

    Rational operator*(const Rational& other) const {
        Rational result = *this;
        result *= other;
        return result;
    }

    Rational operator/(const Rational& other) const {
        Rational result = *this;
        result /= other;
        return result;
    }

    Rational operator+=(const Rational& other) {
        numerator_ *= other.denominator_;
        numerator_ += other.numerator_ * denominator_;
        denominator_ *= other.denominator_;
        Reduction();
        return *this;
    }

    Rational operator-=(const Rational& other) {
        numerator_ *= other.denominator_;
        numerator_ -= other.numerator_ * denominator_;
        denominator_ *= other.denominator_;
        Reduction();
        return *this;
    }

    Rational operator*=(const Rational& other) {
        numerator_ *= other.numerator_;
        denominator_ *= other.denominator_;
        Reduction();
        return *this;
    }

    Rational operator/=(const Rational& other) {
        numerator_ *= other.denominator_;
        denominator_ *= other.numerator_;
        Reduction();
        return *this;
    }

    // Так как нет указателей и ссылок, можно просто скопировать значения не статических полей
    Rational& operator=(const Rational& other) = default;

    // Так как дроби всегда находятся в нормализированном состоянии и нет указателей,
    // нет смысла в сложных расчетах. Достаточно сравнить их нестатические поля
    bool operator==(const Rational& other) const = default;

    auto operator<=>(const Rational& other) const {
        int64_t l_num;
        int64_t r_num;

        // Проверка на равенство выполняется быстрее, чем умножение огромных чисел
        if (other.GetDenominator() != this->GetDenominator()) {
            l_num = this->GetNumerator() * other.GetDenominator();
            r_num = other.GetNumerator() * this->GetDenominator();
        } else {
            l_num = this->GetNumerator();
            r_num = other.GetNumerator();
        }
        return l_num <=> r_num;
    }

    explicit operator double() const {
        return static_cast<double>(numerator_) / denominator_;
    }

    Rational Inv() const {
        if (denominator_ != 0) {
            return {denominator_, numerator_};
        }
        abort();
    }

    int GetNumerator() const {
        return numerator_;
    }

    int GetDenominator() const {
        return denominator_;
    }

     friend std::ostream& operator<<(std::ostream& out, const Rational& r);
     friend std::istream& operator>>(std::istream& in, Rational& r);

private:
    int numerator_ = 0;
    int denominator_ = 1;

    void Reduction() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        const int divisor = std::gcd(numerator_, denominator_);
        numerator_ /= divisor;
        denominator_ /= divisor;
    }
};

// Ввод/вывод
inline std::istream& operator>>(std::istream& in, Rational& r) {
    int num, denom;
    char op;

    in >> num;
    if (in.fail()) {
        return in;
    }

    in >> std::ws >> op;
    if (in.fail()) {
        r = {num, 1};
        in.clear();
        return in;
    }

    if (op != '/') {
        r = {num, 1};
        in.unget();
        return in;
    }

    in >> denom;

    if (in.fail() || denom == 0) {
        in.setstate(std::ios::failbit);
        return in;
    }

    r = {num, denom};
    return in;
}

inline std::ostream& operator<<(std::ostream& out, const Rational& r) {
    out << r.numerator_;
    if (r.denominator_ != 1) {
        out << " / " << r.denominator_;
    }
    return out;
}
