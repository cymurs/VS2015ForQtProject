#ifndef SIGNALDIALOG_H
#define SIGNALDIALOG_H

#include <QDialog>

namespace Ui {
class SignalDialog;
}

class SignalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignalDialog(QWidget *parent = 0);
    ~SignalDialog();

private:
    Ui::SignalDialog *ui;
};

#endif // SIGNALDIALOG_H
