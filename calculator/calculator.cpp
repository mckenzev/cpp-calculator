#include "calculator.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <unordered_set>

bool ReadOperand(Number& first_num, const string& oper) {
    static const std::unordered_map<string, Operation> operations = {
        {"+"s,  [](Number& a, Number b) { a += b; }},
        {"-"s,  [](Number& a, Number b) { a -= b; }},
        {"*"s,  [](Number& a, Number b) { a *= b; }},
        {"/"s,  [](Number& a, Number b) { a /= b; }},
        {":"s,  [](Number& a, Number b) { a = b; }},
        {"**"s, [](Number& a, Number b) { a = std::pow(a, b); }}
    };

    Number second_num;

    // При некорректном вводе завершение программы с ошибкой
    if (!ReadNumber(second_num)) {
        std::cerr << "Error: Numeric operand expected";
        return false;
    }

    // Если прошел некорректный оператор - завершение с ошибкой
    if (!operations.count(oper)) {
        std::cerr << "Error: Unknown token " << oper;
        return false;
    }

    // Тут гарантировано, что из map'а будет взята корректная функция
    auto operation = operations.at(oper);
    operation(first_num, second_num);
    return true;
}

bool IsValidOperation(const string& op) {
	const std::unordered_set<string> operators = 
	{"+"s, "-"s, "*"s, "/"s,  "**"s, ":"s, "="s, "c"s, "s"s, "l"s, "q"s};
	return operators.count(op);
}

bool LoadValue(Number& result, Number& saved_value) {
    if (std::isnan(saved_value)) {
        std::cerr << "Error: Memory is empty" << std::endl;
        return false;
    }
    result = saved_value;
    return true;
}

bool ReadOperator(Number& result, string& oper, bool& waiting_num) {
    // Если равен nan, значит нет значения, иначе - есть
    // static для сохранения результата между вызовами функции
    static Number saved_value = std::numeric_limits<Number>::quiet_NaN();

    std::cin >> oper;
	if (!IsValidOperation(oper)) {
        std::cerr << "Error: Unknown token " << oper;
        return false;
    }

    if (oper == "c"s) {
        result = 0;
        return true;
    }

    if (oper == "s"s) {
        saved_value = result;
        return true;
    }

    if (oper == "l"s) {
        return LoadValue(result, saved_value);
    }

    if (oper == "="s) {
        std::cout << result << std::endl;
        return true;
    }

    // Если дошло до этого момента, значит остались
    // операторы после которыхх водится число
    waiting_num = true;
    return true;
}

bool ReadNumber(Number& result) {
    std::cin >> result;
    return std::cin.good();
}

bool RunCalculatorCycle() {
    Number result;
    string oper;

    if (!ReadNumber(result)) {
        std::cerr << "Error: Numeric operand expected";
        return false;
    }

    // Выбор состояния, ввод оператора или операнда
    bool waiting_num = false;

    // Переменная для накопления ошибок
    bool good_bit = true; 

    while (good_bit) {
        if (waiting_num) { // Ввод числа
            good_bit = ReadOperand(result, oper);
            waiting_num = false;
        } else { // Ввод оператора
            good_bit = ReadOperator(result, oper, waiting_num);

            if (oper == "q"s) {
                break;
            }
        }
    }
    return good_bit;
}
