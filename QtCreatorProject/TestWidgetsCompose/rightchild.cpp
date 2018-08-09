#include "rightchild.h"
#include "ui_right.h"
#include <QStackedLayout>

RightChild::RightChild(QWidget *parent) : QWidget(parent)
  , m_baseLayout(new QStackedLayout(this))
{
//    ui->setupUi(this);

    m_baseLayout->setMargin(0);
    setLayout(m_baseLayout);
}
