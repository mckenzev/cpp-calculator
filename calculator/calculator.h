#pragma once

#include <string>
#include <optional>
#include <cmath>

#include "pow.h"
#include "rational.h"

using Error = std::string;

template <typename Number>
class Calculator {
public:
    void Set(Number n);
    Number GetNumber() const;
    std::optional<Error> Add(Number n);
    std::optional<Error> Sub(Number n);
    std::optional<Error> Div(Number n);
    std::optional<Error> Mul(Number n);
    std::optional<Error> Pow(Number n);
    void SetNegativ();
    void Save();
    void Load();
    void ClearMemory();
    bool GetHasMem() const;
    std::string GetNumberRepr() const;

private:
    Number num_ = 0;
    std::optional<Number> memory_ = std::nullopt;
};

template <typename Number>
void Calculator<Number>::Set(Number n) {
    num_ = n;
}

template <typename Number>
Number Calculator<Number>::GetNumber() const {
    return num_;
}

// Add, Sub, Mul не возвращают ошибок
template <typename Number>
std::optional<Error> Calculator<Number>::Add(Number n) {
    num_ += n;
    return std::nullopt;
}

template <typename Number>
std::optional<Error> Calculator<Number>::Sub(Number n) {
    num_ -= n;
    return std::nullopt;
}

template <typename Number>
std::optional<Error> Calculator<Number>::Mul(Number n) {
    num_ *= n;
    return std::nullopt;
}

// int, uint8_t, size_t, int64_t и Rational при делении на 0 выводят ошибку "Division by zero"
template <typename Number>
std::optional<Error> Calculator<Number>::Div(Number n) {
    // Деление на 0
    if (n == 0) {
        return Error("Division by zero");
    }

    // Корректные случаи
    num_ /= n;
    return std::nullopt;
}

// double и float делятся на 0 без проблем
template <>
inline std::optional<Error> Calculator<double>::Div(double n) {
    num_ /= n;
    return std::nullopt;
}

template <>
inline std::optional<Error> Calculator<float>::Div(float n) {
    num_ /= n;
    return std::nullopt;
}

// int, uint8_t, size_t, int64_t при возведении 0 в степень 0 выводят ошибку "Zero power to zero"
// При возведение в отрицательную степень выводят ошибку "Integer negative power"
template <typename Number>
std::optional<Error> Calculator<Number>::Pow(Number n) {
    // 0 в степени 0
    if (num_ == 0 && n == 0) {
        return Error("Zero power to zero");
    }

    // Отрицательная степень
    if (n < 0) {
        return Error("Integer negative power");
    }

    // Корректные случаи
    num_ = IntegerPow(num_, n);
    return std::nullopt;
}

// Rational при возведении 0 в степень 0 выводят ошибку "Zero power to zero"
// При возведении дроби в дробную степень, выводится ошибка "Fractional power is not supported"
template <>
inline std::optional<Error> Calculator<Rational>::Pow(Rational n) {
    // Возведение 0 в степень 0
    if (num_ == 0 && n == 0) {
        return Error("Zero power to zero");
    }

    // Возведение дроби в дробную степень
    if (n.GetDenominator() != 1) {
        return Error("Fractional power is not supported");
    }

    // Корректные случаи
    num_ = ::Pow(num_, n);
    return std::nullopt;
}

template <>
inline std::optional<Error> Calculator<double>::Pow(double n) {
    num_ = std::pow(num_, n);
    return std::nullopt;
}

template <>
inline std::optional<Error> Calculator<float>::Pow(float n) {
    num_ = std::pow(num_, n);
    return std::nullopt;
}

template <typename Number>
void Calculator<Number>::SetNegativ() {
    num_ = -num_;
}

template <typename Number>
void Calculator<Number>::Save() {
    memory_ = num_;
}

template <typename Number>
void Calculator<Number>::Load() {
    if (!GetHasMem()) {
        return;
    }
    num_ = memory_.value();
}

template <typename Number>
void Calculator<Number>::ClearMemory() {
    memory_ = std::nullopt;
}

template <typename Number>
bool Calculator<Number>::GetHasMem() const {
    return memory_.has_value();
}

template <typename Number>
std::string Calculator<Number>::GetNumberRepr() const {
    return std::to_string(num_);
}
