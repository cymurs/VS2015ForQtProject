#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_lineEdit = new QLineEdit(this);
    m_pushButton = new QPushButton(tr("Send"), this);
    auto hLayout = new QHBoxLayout;

    m_textEdit = new QTextEdit(this);
    auto vLayout = new QVBoxLayout;

    hLayout->addWidget(m_lineEdit, 3);
    hLayout->addWidget(m_pushButton, 1);
    vLayout->addLayout(hLayout, 1);
    vLayout->addWidget(m_textEdit, 5);
    ui->centralWidget->setLayout(vLayout);

    m_portThread.setParams("com10", 19200, 8, 1, 0);
    m_portThread.start();

    setStyleSheet("QMainWindow { background-image: url(:/test/images/bg_update.png); }");
}

MainWindow::~MainWindow()
{
    delete ui;
}
