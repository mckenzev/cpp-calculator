// В этом файле объявления функций.
#pragma once

#include <functional>
#include <string>

using namespace std::literals;
using Number = double;
using string = std::string;
using Operation = std::function<void(Number&, Number)>;

bool ReadNumber(Number& result);
bool RunCalculatorCycle();