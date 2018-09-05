#include "signaldialog.h"
#include "ui_signaldialog.h"

SignalDialog::SignalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignalDialog)
{
    ui->setupUi(this);
}

SignalDialog::~SignalDialog()
{
    delete ui;
}
