#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

Q_SIGNALS:
    void update(qint64, const QString &);

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public Q_SLOTS:
    void execUpdate(qint64, const QString &);
    void onButtonClicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
