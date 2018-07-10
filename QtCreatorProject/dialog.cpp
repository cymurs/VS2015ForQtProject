#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &Dialog::onButtonClicked);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::execUpdate(qint64 num, const QString &content)
{
    QMessageBox::information(Q_NULLPTR, "test", tr("[%1]%2").arg(num).arg(content));
}

void Dialog::onButtonClicked()
{
    bool bconn = connect(this, SIGNAL(update(qint64,QString)), this, SLOT(execUpdate(qint64,QString)));
    QMessageBox::information(Q_NULLPTR, "test", tr("connect update to execUpdate is %1").arg(bconn));
    emit update(1234, tr("update response."));
}
