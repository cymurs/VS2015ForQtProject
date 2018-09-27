#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include "serialportthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    SerialPortThread m_portThread;
    QTextEdit *m_textEdit;
    QLineEdit *m_lineEdit;
    QPushButton *m_pushButton;
};

#endif // MAINWINDOW_H
