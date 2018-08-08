#include "widget.h"
#include "ui_main.h"
#include "topchild.h"
#include "leftchild.h"
#include "rightchild.h"
#include <QHBoxLayout>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_top = new TopChild();
    m_top->resize(ui->frame_3->width(), ui->frame_3->height());
    auto topLayout = new QHBoxLayout(ui->frame_3);
    topLayout->addWidget(m_top);
    topLayout->setMargin(0);
    ui->frame_3->setLayout(topLayout);

    m_left = new LeftChild();
    m_left->resize(ui->frame->width(), ui->frame->height());
    auto leftLayout = new QVBoxLayout(ui->frame);
    leftLayout->addWidget(m_left);
    leftLayout->setMargin(0);
    ui->frame->setLayout(leftLayout);
}

Widget::~Widget()
{
    delete ui;
}
