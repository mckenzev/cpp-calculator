#pragma once

#include <string>

using Number = double;

class Calculator {
public:
    void SetNumber(Number n);
    Number GetNumber() const;
    void Add(Number n);
    void Sub(Number n);
    void Div(Number n);
    void Mul(Number n);
    void Pow(Number n);
    void SetNegativ();
    void Save();
    void Load();
    void ClearMemory();
    bool HasMem() const;
    std::string GetNumberRepr() const;

private:
    Number num_ = 0;
    Number memory_ = 0;
    bool has_mem_ = false;
};

// Реализуйте здесь класс Calculator.
