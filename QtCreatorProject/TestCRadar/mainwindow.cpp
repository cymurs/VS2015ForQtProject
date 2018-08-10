#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cradar.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    CRadar *radar = new CRadar(this);
    radar->resize(400, 400);
//    radar->setFixedSize(400, 400);
    radar->move(50, 50);

    auto baseLayout = new QHBoxLayout(this);
    baseLayout->addWidget(radar);
    baseLayout->setMargin(0);
    setLayout(baseLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}
