#ifndef EMITSTRUCTDIALOG_H
#define EMITSTRUCTDIALOG_H

#include <QDialog>

namespace Ui {
class EmitStructDialog;
}

class EmitStructDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmitStructDialog(QWidget *parent = 0);
    ~EmitStructDialog();

private:
    Ui::EmitStructDialog *ui;
};

#endif // EMITSTRUCTDIALOG_H
