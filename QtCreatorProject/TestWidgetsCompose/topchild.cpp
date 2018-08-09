#include "topchild.h"
#include "ui_top.h"
#include <QResizeEvent>

TopChild::TopChild(QWidget *parent) : QWidget(parent)
  , m_homeBtn(new QPushButton(this))
  , m_backBtn(new QPushButton(this))
{
    ui->setupUi(this);

    int w = this->width();
    int h = this->height();

    m_homeBtn->setStyleSheet( tr(
                       "QPushButton{ " \
                       "width: %1px;" \
                       "height: %2px;" \
                       "border: 1px solid lightgray;" \
                       "border-image: url(:/picture/21.ico); " \
                       "}" \
                       "QPushButton:hover{ border-color: yellow; background: red; }"
                       ).arg(h).arg(h) );
    m_backBtn->setStyleSheet( tr(
                       "QPushButton{ " \
                       "width: %1px;" \
                       "height: %2px;" \
                       "border: 1px solid lightgray;" \
                       "border-image: url(:/picture/24.ico); " \
                       "}" \
                       "QPushButton:hover{ background: rgb(85, 123, 205); }"
                       ).arg(h).arg(h) );
    m_homeBtn->setGeometry(0, 0, h, h);
    m_backBtn->setGeometry(w-h, 0, h, h);
}

void TopChild::resizeEvent(QResizeEvent *event)
{
    QSize s = event->size();
    int w = s.width();
    int h = s.height();

    m_homeBtn->setGeometry(0, 0, h, h);
    m_backBtn->setGeometry(w-h, 0, h, h);
}
