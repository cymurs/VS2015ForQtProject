#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "childwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void closeWindow();

protected:
    bool event(QEvent *event) override;


private:
    Ui::MainWindow *ui;
    ChildWindow *child;
};

#endif // MAINWINDOW_H
