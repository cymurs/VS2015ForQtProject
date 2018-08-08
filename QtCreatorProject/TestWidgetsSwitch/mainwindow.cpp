#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    child(new ChildWindow())
{
    ui->setupUi(this);
    menuBar()->hide(); //ui->menuBar->hide();
    ui->mainToolBar->hide();
    ui->pushButton->setText("quit");
    setWindowFlags(Qt::WindowSystemMenuHint | Qt::FramelessWindowHint ); // Qt::window
    setMouseTracking(true);

    child->resize(this->size());
    child->setWindowTitle(this->windowTitle());    
    child->setMainWindow(this);
    child->hide();

    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(closeWindow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeWindow()
{
    this->close();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
//        QMessageBox::information(this, "tips", "button is clicked.");
        this->hide();
        child->show();
    }

    return true;
}
