#pragma once

#include "calculator.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    enum class Operation {
        NO_OPERATION,   // — операция не задана
        ADDITION,       // — плюс
        SUBTRACTION,    // — минус
        MULTIPLICATION, // — умножить
        DIVISION,       // — поделить
        POWER           // — возведение в степень
    };

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void OperationPressed();
    void NegativePressed();
    void MemoryClearPressed();
    void MemoryLoadPressed();
    void MemorySavePressed();
    void NumberPressed();
    void PointPressed();
    void BackspacePressed();
    void ClearPressed();
    void EqualPressed();

private:
    Ui::MainWindow* ui;
    Calculator calculator_;
    Operation current_operation_ = Operation::NO_OPERATION;
    double active_number_ = 0;
    QString input_number_;
    double memory_cell_;
    bool memory_saved_ = false;

    void InitialSettings() const;
    void AllConnects() const;
    void UpdateLabels() const;
    void UpdateMemoryLabel() const;
    void UpdateFormulaInLabel() const;
    bool IsValidNumber(QString num) const;
};
