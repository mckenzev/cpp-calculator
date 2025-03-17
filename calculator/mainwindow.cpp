#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <unordered_map>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    InitialSettings();
    AllConnects();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::InitialSettings() const {
    ui->l_result->setText("0");
    ui->l_memory->setText("");
    ui->l_formula->setText("");
}

void MainWindow::AllConnects() const {
    // Бинарные операторы
    connect(ui->pb_add, &QPushButton::clicked, this, &MainWindow::OperationPressed);
    connect(ui->pb_sub, &QPushButton::clicked, this, &MainWindow::OperationPressed);
    connect(ui->pb_mul, &QPushButton::clicked, this, &MainWindow::OperationPressed);
    connect(ui->pb_div, &QPushButton::clicked, this, &MainWindow::OperationPressed);
    connect(ui->pb_pow, &QPushButton::clicked, this, &MainWindow::OperationPressed);

    // Унарные операторы
    connect(ui->pb_negative, &QPushButton::clicked, this, &MainWindow::NegativePressed);

    // Работа с memory
    connect(ui->pb_memory_clear, &QPushButton::clicked, this, &MainWindow::MemoryClearPressed);
    connect(ui->pb_memory_load, &QPushButton::clicked, this, &MainWindow::MemoryLoadPressed);
    connect(ui->pb_memory_save, &QPushButton::clicked, this, &MainWindow::MemorySavePressed);

    // Цифры
    connect(ui->pb_0, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_1, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_2, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_3, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_4, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_5, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_6, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_7, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_8, &QPushButton::clicked, this, &MainWindow::NumberPressed);
    connect(ui->pb_9, &QPushButton::clicked, this, &MainWindow::NumberPressed);

    // Точка, backspace
    connect(ui->pb_point, &QPushButton::clicked, this, &MainWindow::PointPressed);
    connect(ui->pb_backspace, &QPushButton::clicked, this, &MainWindow::BackspacePressed);

    // Очистка, равно
    connect(ui->pb_clear, &QPushButton::clicked, this, &MainWindow::ClearPressed);
    connect(ui->pb_equal, &QPushButton::clicked, this, &MainWindow::EqualPressed);
}


void MainWindow::UpdateLabels() const {
    UpdateFormulaInLabel();
    UpdateResultLabel();
}

void MainWindow::UpdateMemoryLabel() const {
    if (memory_saved_) {
        ui->l_memory->setText("M");
    } else {
        ui->l_memory->setText("");
    }
}

void MainWindow::UpdateFormulaInLabel() const {
    static const std::unordered_map<Operation, QString> OPERATIONS = {
        {Operation::NO_OPERATION, ""},
        {Operation::ADDITION, " + "},
        {Operation::SUBTRACTION, " − "},
        {Operation::MULTIPLICATION, " × "},
        {Operation::DIVISION, " ÷ "},
        {Operation::POWER, " ^ "}
    };

    auto operation = OPERATIONS.at(current_operation_);

    if (operation.isEmpty()) {
        ui->l_formula->setText("");
        ui->l_result->setText(input_number_);
    } else {
        ui->l_formula->setText(QString("%1%2")
                                    .arg(active_number_)
                                    .arg(operation));
    }
}

void MainWindow::UpdateResultLabel() const {
    QString l_result_text = input_number_.isEmpty()
                          ? QString::number(active_number_)
                          : input_number_;


    ui->l_result->setText(l_result_text);
}

bool MainWindow::IsValidNumber(QString num) const {
    // Пустое число - валидно
    // Если число не начинается с 0, или начинается с 0, но содержит в себе точку,
    // значит это число валидно

    return (num.isEmpty() || num.front() != '0' || num.contains('.'));
}

void MainWindow::BackspaceProcessingFirstOperand() {
    // Если input_number_ пуст(на выводе active_number_) или его длина равна 1 символу, значит
    // после нажатия на backespace состояние программы должно быть как на старте работы программы (но memory_cell_ не трогаем)
    if (input_number_.isEmpty() || input_number_.size() == 1) {
        ClearPressed();
        return;
    }

    // Иначе удаляется только 1 символ из input_number_
    auto new_size = input_number_.size() - 1;
    input_number_.resize(new_size);
}

void MainWindow::BackspaceProcessingSecondOperand() {

    // У пустого input_number_ нечего удалять
    if (input_number_.isEmpty()) {
        return;
    }

    // Удаление 1 символа с конца
    auto new_size = input_number_.size() - 1;
    input_number_.resize(new_size);
}

// Slots
void MainWindow::OperationPressed() {
    static const std::unordered_map<QString, Operation> OPERATIONS = {
        {"+", Operation::ADDITION},
        {"−", Operation::SUBTRACTION},
        {"×", Operation::MULTIPLICATION},
        {"÷", Operation::DIVISION},
        {"xʸ", Operation::POWER}
    };
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString text = button->text();

    // Ввод цифр работает только с input_number_. Если слот вызвался для первичного ввода оператора, значит содержимое
    // input_number_ - первый операнд. Но если же input_number_ пуст, то для первого операнда берется значение из active_number_
    // (в нем либо результат вычисления, либо 0 после нажатия на pb_clear, либо выгруженное значение из memory_cell_)
    // т.о. после нажатия равенства можно сразу же оперировать с результатом, добавив к нему оператор, или вместо него вводить другой "Первый операнд".
    // Во всех других случаях меняется только оператор, а операнды остаются неизменны
    if (current_operation_ == Operation::NO_OPERATION && !input_number_.isEmpty()) {
        active_number_ = input_number_.toDouble();
        input_number_.clear();
    }

    current_operation_ = OPERATIONS.at(text);

    UpdateFormulaInLabel();
}

void MainWindow::MemoryClearPressed() {
    memory_saved_ = false;
    UpdateMemoryLabel();
}

void MainWindow::MemoryLoadPressed() {
    if (!memory_saved_) {
        return;
    }

    input_number_.clear();
    active_number_ = memory_cell_;
    UpdateLabels();
}

void MainWindow::MemorySavePressed() {
    memory_cell_ = active_number_;
    memory_saved_ = true;
    UpdateMemoryLabel();
}

void MainWindow::NumberPressed() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString text = button->text();
    if (IsValidNumber(input_number_)) {
        input_number_ += text;
    }

    if (current_operation_ == Operation::NO_OPERATION) {
        active_number_ = input_number_.toDouble();
    }
    UpdateLabels();
}

void MainWindow::PointPressed() {
    if (input_number_.isEmpty()) {
        input_number_ += "0.";
    } else if (!input_number_.contains('.')) {
        input_number_ += '.';
    }
    UpdateLabels();
}

void MainWindow::BackspacePressed() {
    // Если current_operation_ равен Operation::NO_OPERATION,
    // значит сейчас редактируется первый операнд
    bool its_first_operand = current_operation_ == Operation::NO_OPERATION;
    if (its_first_operand) {
        BackspaceProcessingFirstOperand();
    } else {
        BackspaceProcessingSecondOperand();
    }
    UpdateLabels();
}

void MainWindow::NegativePressed() {
    if (input_number_.isEmpty()) {
        return;
    }

    auto number = ui->l_result->text().toDouble();
    calculator_.SetNumber(number);
    calculator_.SetNegativ();
    input_number_ = QString::number(calculator_.GetNumber());
    UpdateLabels();
}

void MainWindow::ClearPressed() {
    input_number_.clear();
    active_number_ = 0;
    current_operation_ = Operation::NO_OPERATION;

    ui->l_result->setText("0");
    ui->l_formula->setText("");
}

void MainWindow::EqualPressed() {
    if (current_operation_ == Operation::NO_OPERATION) {
        return;
    }

    // Обработка отсутствия ввода второго числа
    // Если второй операнд не введен, значит в качестве него
    // будет то же значение, что и у первого операнда
    if (input_number_.isEmpty()) {
        input_number_ = QString::number(active_number_);
    }

    double second_num = input_number_.toDouble();
    calculator_.SetNumber(active_number_);
    switch(current_operation_) {
        case Operation::ADDITION :
            calculator_.Add(second_num);
            break;
        case Operation::SUBTRACTION :
            calculator_.Sub(second_num);
            break;
        case Operation::MULTIPLICATION :
            calculator_.Mul(second_num);
            break;
        case Operation::DIVISION :
            calculator_.Div(second_num);
            break;
        case Operation::POWER :
            calculator_.Pow(second_num);
            break;
        case Operation::NO_OPERATION:
            // Эта строка не нужна, т.к. проверка производится в самом начале, но IDE кидает варнинги без этой строки
            break;
        }

    auto formula_text = ui->l_formula->text();
    ui->l_formula->setText(formula_text + input_number_ + " =");
    active_number_ = calculator_.GetNumber();
    current_operation_ = Operation::NO_OPERATION;
    ui->l_result->setText(QString::number(active_number_));
    input_number_.clear();
}
