#include "calculator.h"

#include <cmath>

void Calculator::SetNumber(Number n) {
    num_ = n;
}

Number Calculator::GetNumber() const {
    return num_;
}

void Calculator::Add(Number n) {
    num_ += n;
}

void Calculator::Sub(Number n) {
    num_ -= n;
}

void Calculator::Div(Number n) {
    num_ /= n;
}

void Calculator::Mul(Number n) {
    num_ *= n;
}

void Calculator::Pow(Number n) {
    num_ = std::pow(num_, n);
}

void Calculator::SetNegativ() {
    num_ = -num_;
}

void Calculator::Save() {
    memory_ = num_;
    has_mem_ = true;
}

void Calculator::Load() {
    num_ = memory_;
}

void Calculator::ClearMemory() {
    has_mem_ = false;
}

bool Calculator::HasMem() const {
    return has_mem_;
}

std::string Calculator::GetNumberRepr() const {
    return std::to_string(num_);
}
