#include "topchild.h"
#include "ui_top.h"

TopChild::TopChild(QWidget *parent) : QWidget(parent)
  , m_homeBtn(new QPushButton(this))
  , m_backBtn(new QPushButton(this))
{
    ui->setupUi(this);

    int w = this->width();
    int h = this->height();

    m_homeBtn->setStyleSheet( tr(
                       "QPushButton{ " \
                       "min-width: %1px;" \
                       "min-height: %2px;" \
                       "border: 1px solid lightgray;" \
                       "border-image: url(:/picture/21.ico); " \
                       "}" \
                       "QPushButton:hover{ border-color: yellow; background: red; }"
                       ).arg(h).arg(h) );
    m_backBtn->setStyleSheet( tr(
                       "QPushButton{ " \
                       "min-width: %1px;" \
                       "min-height: %2px;" \
                       "border: 1px solid lightgray;" \
                       "border-image: url(:/picture/24.ico); " \
                       "}" \
                       "QPushButton:hover{ background: rgb(85, 123, 205); }"
                       ).arg(h).arg(h) );
    m_homeBtn->setGeometry(0, 0, h, h);
    m_backBtn->setGeometry(w-h, 0, h, h);
}
