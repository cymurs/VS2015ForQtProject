#include "leftchild.h"
#include "ui_left.h"
#include "qss.h"
#include <QVBoxLayout>
#include <QMessageBox>


LeftChild::LeftChild(QWidget *parent) : QWidget(parent)
  , m_timeSrc(new QPushButton(tr("时间源"), this))
  , m_serialPort(new QPushButton(tr("串口设置"), this))
  , m_network(new QPushButton(tr("网络设置"), this))
  , m_state(new QPushButton(tr("状态参数"), this))
  , m_curPressed(Q_NULLPTR)
{
    //ui->setupUi(this);

    m_timeSrc->setObjectName("LeftButtonTimeSrc");
    m_serialPort->setObjectName("LeftButtonSerialPort");
    m_network->setObjectName("LeftButtonNetwork");
    m_state->setObjectName("LeftButtonState");

    connect(m_timeSrc, &QPushButton::clicked, [=](){
        QMessageBox::information(nullptr, tr("信息"), tr("时间源按钮被按下"));
    });

    setStyleSheet(LeftButtonEnabled);
    connect(m_timeSrc, &QPushButton::pressed, [=](){
        if (Q_NULLPTR != m_curPressed)
            m_curPressed->setStyleSheet("");
        m_curPressed = m_timeSrc;
        changeStyleSheet(m_curPressed);
    });
    connect(m_serialPort, &QPushButton::pressed, [=](){
        if (Q_NULLPTR != m_curPressed)
            m_curPressed->setStyleSheet("");
//        setStyleSheet(LeftButtonEnabled);
//        QString style(LeftButtonPressed.arg(m_serialPort->objectName()));
//        m_serialPort->setStyleSheet(style);
        m_curPressed = m_serialPort;
        changeStyleSheet(m_curPressed);
    });
    connect(m_network, &QPushButton::pressed, [=](){
        if (Q_NULLPTR != m_curPressed)
            m_curPressed->setStyleSheet("");
        m_curPressed = m_network;
        changeStyleSheet(m_curPressed);
    });
    connect(m_state, &QPushButton::pressed, [=](){
        if (Q_NULLPTR != m_curPressed)
            m_curPressed->setStyleSheet("");
        m_curPressed = m_state;
        changeStyleSheet(m_curPressed);
    });

    auto vLayout = new QVBoxLayout(this);
    vLayout->addWidget(m_timeSrc);
    vLayout->addWidget(m_serialPort);
    vLayout->addWidget(m_network);
    vLayout->addWidget(m_state);
    vLayout->setMargin(0);
    setLayout(vLayout);

//    int w = width();
//    int h = height();
//    int btnHeight = h / 4;
//    m_timeSrc->setGeometry(0, 0, w, btnHeight);
//    m_serialPort->setGeometry(0, btnHeight, w, btnHeight);
//    m_network->setGeometry(0, btnHeight*2, w, btnHeight);
//    m_state->setGeometry(0, h-btnHeight, w, btnHeight);
}

void LeftChild::changeStyleSheet(QPushButton *btn)
{
    setStyleSheet(LeftButtonEnabled);
    QString style(LeftButtonPressed.arg(btn->objectName()));
    btn->setStyleSheet(style);
}
