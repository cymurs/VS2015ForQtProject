#include "stateparam.h"
#include "ui_stateparam.h"

StateParam::StateParam(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StateParam)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &StateParam::bdsClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &StateParam::logClicked);
}

StateParam::~StateParam()
{
    delete ui;
}
