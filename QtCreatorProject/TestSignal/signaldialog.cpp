#include "signaldialog.h"
#include "ui_signaldialog.h"
#include <QMessageBox>

SignalDialog::SignalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignalDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(signalStruct(st_Signal,QString)), this, SLOT(slotOnStructSignal(st_Signal,QString)));
}

SignalDialog::~SignalDialog()
{
    delete ui;
}

void SignalDialog::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    st_Signal st{1, "test"};
    emit signalStruct(st, "first");
}

void SignalDialog::slotOnStructSignal(const SignalDialog::st_Signal &st, const QString &desc)
{
    QMessageBox::information(Q_NULLPTR, desc, tr("[%1]%2").arg(st.num).arg(st.str));
}
