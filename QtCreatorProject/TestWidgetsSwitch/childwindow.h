#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <QWidget>
#include <QPushButton>

class MainWindow;

namespace Ui {
class ChildWindow;
}

class ChildWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ChildWindow(QWidget *parent = nullptr);

    void setMainWindow(MainWindow *mainWnd);

signals:

public slots:
    void backMainWindow();

private:
    Ui::ChildWindow *ui;
    QPushButton *homeBtn;
    QPushButton *backBtn;
    MainWindow *mainWnd;
};

#endif // CHILDWINDOW_H
