#pragma once

#include "enums.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    // Изменение состояния лейблов
    void SetInputText(const std::string& text);
    void SetErrorText(const std::string& text);
    void SetFormulaText(const std::string& text);
    void SetMemText(const std::string& text);
    void SetExtraKey(const std::optional<std::string>& key);

    // Установка колбек функций
    void SetDigitKeyCallback(std::function<void(int key)> cb);
    void SetProcessOperationKeyCallback(std::function<void(Operation key)> cb);
    void SetProcessControlKeyCallback(std::function<void(ControlKey key)> cb);
    void SetControllerCallback(std::function<void(ControllerType controller)> cb);

private:
    Ui::MainWindow* ui;

    std::function<void(int key)> digit_key_callback_;
    std::function<void(Operation key)> process_operation_key_callback_;
    std::function<void(ControlKey key)> process_control_key_callback_;
    std::function<void(ControllerType controller)> controller_callback_;

    // Коннекты по группам кнопок
    void AllConects();
    void DigitConects();
    void OperationConects();
    void ControlConects();

private slots:
    void DigitPressed(int);
    void OperationPressed(int);
    void ControlPressed(int);
    void ChangeType();
};
