#include "widget.h"
#include "ui_main.h"
#include "topchild.h"
#include "leftchild.h"
#include "rightchild.h"
#include <QHBoxLayout>
#include <QResizeEvent>

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
//    m_left->setButtonSize(ui->frame->width(), ui->frame->height()/4);
    auto leftLayout = new QVBoxLayout(ui->frame);
    leftLayout->addWidget(m_left);
    leftLayout->setMargin(0);
    leftLayout->setSpacing(0);
    ui->frame->setLayout(leftLayout);

    auto baseLayout = new QGridLayout(this);
    baseLayout->addWidget(ui->frame_3, 0, 0, 1, 2);
    baseLayout->addWidget(ui->frame, 1, 0);
    baseLayout->addWidget(ui->frame_2, 1, 1);
    baseLayout->setRowStretch(0, 1);
    baseLayout->setRowStretch(1, 8);
    baseLayout->setColumnStretch(0, 1);
    baseLayout->setColumnStretch(1, 4);
    baseLayout->setMargin(0); // 设置子控件与主窗口之间的间距
    baseLayout->setSpacing(0); // 设置子空间之间的间距
    setLayout(baseLayout);


    resize(408, 362); // 调整数值，去除子窗口内部button之间的空白
    setMinimumSize(408, 362);
    setSizeIncrement(10, 18); // 保证等比例缩放
}

Widget::~Widget()
{
    delete ui;
}

void Widget::resizeEvent(QResizeEvent *event)
{
    if (event->type() == QEvent::Resize) {
        QSize s = event->size();
        m_top->resize(ui->frame_3->width(), ui->frame_3->height());
//        m_left->setButtonSize(ui->frame->width(), ui->frame->height()/4);
        m_left->resize(ui->frame->width(), ui->frame->height());
    }
}
