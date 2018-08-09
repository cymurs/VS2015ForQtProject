#ifndef STATEPARAM_H
#define STATEPARAM_H

#include <QWidget>

namespace Ui {
class StateParam;
}

class StateParam : public QWidget
{
    Q_OBJECT

public:
    explicit StateParam(QWidget *parent = 0);
    ~StateParam();

signals:
    void bdsClicked(bool checked = false);
    void logClicked(bool checked = false);

private:
    Ui::StateParam *ui;
};

#endif // STATEPARAM_H
