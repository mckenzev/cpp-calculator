#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <unordered_map>

#include <QButtonGroup>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->cmb_controller->addItems({
        "double",
        "float",
        "uint8_t",
        "int",
        "int64_t",
        "size_t",
        "Rational"
    });
    AllConects();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::SetInputText(const std::string& text) {
    // При замене стиля на "" сбивается размер шрифта
    ui->l_result->setStyleSheet("font: 14pt \"Segoe UI\";\n"
                                "color: black;");
    ui->l_result->setText(QString::fromStdString(text));
}

void MainWindow::SetErrorText(const std::string& text) {
    ui->l_result->setStyleSheet("font: 14pt \"Segoe UI\";\n"
                                "color: red;");
    ui->l_result->setText(QString::fromStdString(text));
}

void MainWindow::SetFormulaText(const std::string& text) {
    ui->l_formula->setText(QString::fromStdString(text));
}

void MainWindow::SetMemText(const std::string& text) {
    ui->l_memory->setText(QString::fromStdString(text));
}

void MainWindow::SetExtraKey(const std::optional<std::string>& key) {
    if (!key.has_value()) {
        ui->tb_extra->hide();
        return;
    }
    ui->tb_extra->show();
    ui->tb_extra->setText(QString::fromStdString(key.value()));
}

void MainWindow::SetDigitKeyCallback(std::function<void(int key)> cb) {
    if (cb == nullptr) {
        return;
    }
    digit_key_callback_ = cb;
}

void MainWindow::SetProcessOperationKeyCallback(std::function<void(Operation key)> cb) {
    if (cb == nullptr) {
        return;
    }
    process_operation_key_callback_ = cb;
}

void MainWindow::SetProcessControlKeyCallback(std::function<void(ControlKey key)> cb) {
    if (cb == nullptr) {
        return;
    }
    process_control_key_callback_ = cb;
}

void MainWindow::SetControllerCallback(std::function<void(ControllerType controller)> cb) {
    if (cb == nullptr) {
        return;
    }
    controller_callback_ = cb;
}

// Коннекты
void MainWindow::AllConects() {
    DigitConects();
    OperationConects();
    ControlConects();
    connect(ui->cmb_controller, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeType()));
}

void MainWindow::DigitConects() {
    QButtonGroup *digit_keys = new QButtonGroup(this);

    digit_keys->addButton(ui->pb_0, 0);
    digit_keys->addButton(ui->pb_1, 1);
    digit_keys->addButton(ui->pb_2, 2);
    digit_keys->addButton(ui->pb_3, 3);
    digit_keys->addButton(ui->pb_4, 4);
    digit_keys->addButton(ui->pb_5, 5);
    digit_keys->addButton(ui->pb_6, 6);
    digit_keys->addButton(ui->pb_7, 7);
    digit_keys->addButton(ui->pb_8, 8);
    digit_keys->addButton(ui->pb_9, 9);

    connect(digit_keys, SIGNAL(idClicked(int)), this, SLOT(DigitPressed(int)));
}

void MainWindow::OperationConects() {
    QButtonGroup* operation_keys = new QButtonGroup(this);

    operation_keys->addButton(ui->pb_add, static_cast<int>(Operation::ADDITION));
    operation_keys->addButton(ui->pb_sub, static_cast<int>(Operation::SUBTRACTION));
    operation_keys->addButton(ui->pb_mul, static_cast<int>(Operation::MULTIPLICATION));
    operation_keys->addButton(ui->pb_div, static_cast<int>(Operation::DIVISION));
    operation_keys->addButton(ui->pb_pow, static_cast<int>(Operation::POWER));

    connect(operation_keys, SIGNAL(idClicked(int)), this, SLOT(OperationPressed(int)));
}

void MainWindow::ControlConects() {
    QButtonGroup* control_keys = new QButtonGroup(this);

    control_keys->addButton(ui->pb_equal, static_cast<int>(ControlKey::EQUALS));
    control_keys->addButton(ui->pb_clear, static_cast<int>(ControlKey::CLEAR));
    control_keys->addButton(ui->pb_memory_save, static_cast<int>(ControlKey::MEM_SAVE));
    control_keys->addButton(ui->pb_memory_load, static_cast<int>(ControlKey::MEM_LOAD));
    control_keys->addButton(ui->pb_memory_clear, static_cast<int>(ControlKey::MEM_CLEAR));
    control_keys->addButton(ui->pb_negative, static_cast<int>(ControlKey::PLUS_MINUS));
    control_keys->addButton(ui->pb_backspace, static_cast<int>(ControlKey::BACKSPACE));
    control_keys->addButton(ui->tb_extra, static_cast<int>(ControlKey::EXTRA_KEY));

    connect(control_keys, SIGNAL(idClicked(int)), this, SLOT(ControlPressed(int)));
}

// Слоты
void MainWindow::DigitPressed(int num) {
    if (digit_key_callback_ == nullptr) {
        return;
    }
    digit_key_callback_(num);
}

void MainWindow::OperationPressed(int id) {
    if (process_operation_key_callback_ == nullptr) {
        return;
    }
    process_operation_key_callback_(static_cast<Operation>(id));
}

void MainWindow::ControlPressed(int id) {
    if (process_control_key_callback_ == nullptr) {
        return;
    }
    process_control_key_callback_(static_cast<ControlKey>(id));
}

void MainWindow::ChangeType() {
    if (controller_callback_ == nullptr) {
        return;
    }

    static const std::unordered_map<QString, ControllerType> kTypes = {
        {"double", ControllerType::DOUBLE},
        {"float", ControllerType::FLOAT},
        {"uint8_t", ControllerType::UINT8_T},
        {"int", ControllerType::INT},
        {"int64_t", ControllerType::INT64_T},
        {"size_t", ControllerType::SIZE_T},
        {"Rational", ControllerType::RATIONAL}
    };
    ControllerType type = kTypes.at(ui->cmb_controller->currentText());
    controller_callback_(type);
}
