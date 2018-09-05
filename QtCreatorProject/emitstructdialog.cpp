#include "emitstructdialog.h"
#include "ui_emitstructdialog.h"

EmitStructDialog::EmitStructDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmitStructDialog)
{
    ui->setupUi(this);
}

EmitStructDialog::~EmitStructDialog()
{
    delete ui;
}
