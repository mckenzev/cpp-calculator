#pragma once

#include <string>
#include <sstream>
#include "calculator.h"
#include "mainwindow.h"
#include "rational.h"

// Переводит значение в строку
template<class T>
std::string ToString(T number) {
    std::ostringstream tmp_str;
    tmp_str << number;
    return tmp_str.str();
}

// Переводит строку в значение
template<class T>
T FromString(const std::string& number) {
    std::istringstream tmp_str{number};
    T result{};
    tmp_str >> result;
    return result;
}

// Специализация перевода uint-t в строку, т.к. по дефолту это char
inline std::string ToString(std::uint8_t number) {
    std::ostringstream tmp_str;
    tmp_str << +number;
    return tmp_str.str();
}

// Специализация перевода стркои в uint8_t
template<>
std::uint8_t FromString<std::uint8_t>(const std::string& number) {
    std::istringstream tmp_str{number};
    unsigned result{};
    tmp_str >> result;
    return static_cast<std::uint8_t>(result);
}

template<class Number>
class Controller {
public:
    Controller() {}

    // Связывание контроллера с вью по указателю
    void BindWithMainWindow(MainWindow* main_window) {
        view_ = main_window;
        // Если view_ == nullptr, то завершить работу метода
        if (!view_) {
            return;
        }

        // связывание вью с колбек функцией КОНКРЕТНОГО контроллера с КОНКРЕТНОЙ специализацией
        view_->SetDigitKeyCallback([this](int key){
            PressDigitKey(key);
        });
        view_->SetProcessOperationKeyCallback([this](Operation key){
            ProcessOperationKey(key);
        });
        view_->SetProcessControlKeyCallback([this](ControlKey key){
            ProcessControlKey(key);
        });

        // Показать вью. Устанавливает значения в соответствующие поля лейблов
        RevealView();
    }

private:
    // Считывание нажатия цифры на циферблате
    void PressDigitKey(int digit) {
        AddChar(digit + '0');
    }

    // Обработка кнопок операций
    // operation определен enum'ом из Operation, отвечающим за 1 из 5 операций
    void ProcessOperationKey(Operation operation) {
        switch(operation) {
        case Operation::ADDITION:
            OnOperation([this](Number x){return calculator_.Add(x);}, " + ");
            break;
        case Operation::SUBTRACTION:
            OnOperation([this](Number x){return calculator_.Sub(x);}, " − ");
            break;
        case Operation::MULTIPLICATION:
            OnOperation([this](Number x){return calculator_.Mul(x);}, " × ");
            break;
        case Operation::DIVISION:
            OnOperation([this](Number x){return calculator_.Div(x);}, " ÷ ");
            break;
        case Operation::POWER:
            OnOperation([this](Number x){return calculator_.Pow(x);}, " ^ ");
            break;
        }
    }

    // Обработка контрол кнопок
    void ProcessControlKey(ControlKey key) {
        switch(key) {
        case ControlKey::EQUALS: { // равенство
            // Если операция не была введена, ничего не происходит
            if (!operation_) {
                return;
            }
            // Текст формулы формируется как содержимое калькулятора(1 оп) + оператор + актичное число(2 оп) + "="
            std::string formula = ToString(calculator_.GetNumber()) + operation_name_ + ToString(active_number_) + " = ";

            // При совершении операции может быть возвращен текст ошибки
            auto error = operation_(active_number_);
            // Если ошибка была получена, ее текст отправляется во вью и свитч-кейс прерывает работу
            if (error.has_value()) {
                SetErrorInView(error.value());
                break;
            }
            // Обновляется текст формулы в l_formula
            SetFormulaInView(formula);
            // Результат вычисления отправляется в l_result
            SetInputAsNumber(calculator_.GetNumber());
            // Лямбда операции зануляется
            operation_ = {};
            break;
        }
        case ControlKey::CLEAR: // Очистка
            SetInputAsNumber(Number{});
            SetFormulaInView("");
            operation_ = {};
            break;
        case ControlKey::MEM_SAVE: // сохранение активного числа в памяти
            mem_ = active_number_;
            SetMemInView("M");
            break;
        case ControlKey::MEM_LOAD: // Выгрузка значения из памяти
            // Если память пуста, ничего не происходит
            if (!mem_.has_value()) {
                return;
            }
            // в active_number_ отправляется значение из памяти и обновляется l_result
            SetInputAsNumber(mem_.value());
            // Формула очищается
            SetFormulaInView("");
            break;
        case ControlKey::MEM_CLEAR: // Удаление значения из памяти
            mem_ = std::nullopt;
            // Очистка индикатора "не пустоты" памяти
            SetMemInView("");
            break;
        case ControlKey::PLUS_MINUS: // Унарный минус
            // Если активное число не является input_, то изменения знака касаются active_number_
            if (input_as_number_) {
                active_number_ = -active_number_;
                SetInputAsNumber(active_number_);
            } else { // Иначе модифицируем input_
                // Если строка не пуста и начинается с минуса, оставляем подстроку с 1 символа(все после минуса)
                if (input_.size() && input_.front() == '-') {
                    SetInputAsString(input_.substr(1));
                } else { // Иначе ставим минус перед неотрицательной строкой(или минус перед пустой строкой)
                    SetInputAsString("-" + input_);
                }
            }
            break;
        case ControlKey::BACKSPACE: // Бэкспейс
            // пока инпут не пуст можно выполнять посимвольное удаление
            if (input_.size() > 0) {
                // Передаем в l_result имеющуюся строку без последнего символа
                SetInputAsString(input_.substr(0, input_.size() - 1));
            }
            break;
        case ControlKey::EXTRA_KEY: // Точка/слэш
            DoExtraAction();
            break;
        }
    }

    // Возвращает текст экстра кнопки
    std::optional<std::string> GetExtraKey() const {
        if constexpr (std::is_integral_v<Number>) {
            return std::nullopt;
        }
        if constexpr (std::is_same_v<Number, Rational>) {
            return "/";
        }
        return ".";
    }


    void OnOperation(std::function<std::optional<Error> (Number)> action, std::string text) {
        // Если изначально указатель на функцию был равен nullptr, в качестве стартового значения калькулятора устанавливает active_number_(дефолтное значение используемого типа)
        if (!operation_) {
            calculator_.Set(active_number_);
        }

        // в operation_name_ устанавливается строка с названием операции
        operation_name_ = text;
        // К лямбде operation_ присваивается функтор с операцией
        operation_ = action;
        // Инпут обнуляется в ожидании ввода второго оператора
        input_ = "";

        // В l_formula записывается не старый input_, а именно число, которое хранится в calculator_ и оператор
        SetFormulaInView(ToString(calculator_.GetNumber()) + text);
    }

    // Передает в input_ строку
    void SetInputAsString(const std::string& new_input) {
        // Т.к. в инпут передал строку, значит в active_number_ не результат вычисления
        input_as_number_ = false;
        input_ = new_input;
        // Записывает в active_number_ текущее число
        active_number_ = FromString<Number>(input_);
        // Обновляет l_result
        UpdateInputInView(new_input);
    }

    // Передает в input_ значение, которое преобразуется в строку перед выводом
    void SetInputAsNumber(Number number) {
        // Т.к. active_number_ заполнен по значению, значит это результат вычисления или выгрузка из памяти
        input_as_number_ = true;
        // Инпут обнулен, т.к. active_number_ получен в результате вычисления/выгрузки из памяти вычисления
        input_ = "";
        active_number_ = number;
        // Обновление l_result
        UpdateInputInView(ToString(active_number_));
    }

    // Передает в l_result имеющийся в нем текст + символ("123" + '4' = "1234")
    void AddChar(char character) {
        SetInputAsString(input_ + character);
    }

    // Обарабатывает нажатие экстра кнопки
    void DoExtraAction() {
        // Для интовых типов эта кнопка ничего не делает
        if constexpr (std::is_integral_v<Number>) {
            return;
        }
        // Для рациональных чисел выводит '/' в случае, если input_ не пуст и '/' еще не встречался в числе
        if constexpr (std::is_same_v<Number, Rational>) {
            if (input_.size() == 0 || input_.find('/') != std::string::npos) {
                return;
            }
            AddChar('/');
        // Для чисел с точкой, точка выводится только в случае, если она не была введена ранее
        } else {
            if (input_.find('.') != std::string::npos) {
                return;
            }
            AddChar('.');
        }
    }

    // Передает в l_result текст с вводимым числом
    void UpdateInputInView(const std::string& string) {
        if (!view_) {
            return;
        }
        view_->SetInputText(string);
        text_ = string;
    }

    // Выводит текст ошибки
    void SetErrorInView(const std::string& string) {
        if (!view_) {
            return;
        }
        view_->SetErrorText(string);
    }

    // Обновляет текст в l_formula. string - строка формулы
    void SetFormulaInView(const std::string& string) {
        // Если вью равен nullptr, ничего не делаем
        if (!view_) {
            return;
        }
        // Передает полученную строку в l_formula
        view_->SetFormulaText(string);
        // Запоминает текущее состояние l_formula в formula_
        formula_ = string;
    }

    // Обновляет текст в l_memory. string - индикатор содержимого памяти
    void SetMemInView(const std::string& string) {
        // Если вью равен nullptr, ничего не делаем
        if (!view_) {
            return;
        }
        // Передает полученный индикатор в l_memory
        view_->SetMemText(string);
        // Запоминает состояние памяти в mem_text_
        mem_text_ = string;
    }

    // Обновляет текст всех лейблов и экстра кнопки
    void RevealView() {
        if (!view_) {
            return;
        }
        view_->SetInputText(text_);
        view_->SetFormulaText(formula_);
        view_->SetMemText(mem_text_);
        view_->SetExtraKey(GetExtraKey());
    }

private:
    std::function<std::optional<Error>(Number number)> operation_;  // Функция, для вызова функции текущего оператора
    std::string operation_name_;                                    // Название(символ) введенного оператора

    Calculator<Number> calculator_;                                 // Обьект калькулятора конкретной специализации

    Number active_number_ = {};                                     // Введенное число в формате числа
    std::string input_;                                             // Строка с вводимым числом

    MainWindow* view_ = nullptr;                                    // Указатель на вью

    std::optional<Number> mem_ = {};                                // Обьект optional с (не)содержащимся в нем значением, отвечающий за память

    bool input_as_number_ = true;                                   // Флаг контроля содержания active_number_ в input_ или только в active_number_
                                                                    // (active_number_ заполнилось от input или был получен как результат вычисления/возврат значения из памяти)
                                                                    // true - active_number_ получен в результате вычисления; false - active_number_ это input_;

    std::string text_;                                              // Текст l_result
    std::string formula_;                                           // Текст l_formula
    std::string mem_text_;                                          // Текст l_memory
};
