#include "childwindow.h"
#include "ui_childwindow.h"
#include "mainwindow.h"

ChildWindow::ChildWindow(QWidget *parent) : QWidget(parent)
  , homeBtn(new QPushButton(this))
  , backBtn(new QPushButton("", this))
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint );


    homeBtn->setStyleSheet("QPushButton{ " \
                       "min-width: 64px;" \
                       "min-height: 64px;" \
                       "border: 1px solid lightgray;" \
                       "border-image: url(:/picture/21.ico); " \
                       "}" \
                       "QPushButton:hover{ border-color: yellow; background: red; }" );
    backBtn->setStyleSheet("QPushButton{ " \
                       "min-width: 64px;" \
                       "min-height: 64px;" \
                       "border: 1px solid lightgray;" \
                       "border-image: url(:/picture/24.ico); " \
                       "}" \
                       "QPushButton:hover{ background: rgb(85, 123, 205); }" );

    connect(homeBtn, SIGNAL(clicked(bool)), this, SLOT(backMainWindow()));
    connect(backBtn, SIGNAL(clicked(bool)), this, SLOT(backMainWindow()));
}

void ChildWindow::setMainWindow(MainWindow *mainWnd)
{
    this->mainWnd = mainWnd;
    homeBtn->setGeometry(0, 0, 64, 64);
    backBtn->setGeometry(this->rect().width()-64, 0, 64, 64);
}

void ChildWindow::backMainWindow()
{
    this->hide();
    mainWnd->show();
}
