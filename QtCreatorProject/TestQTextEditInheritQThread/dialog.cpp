#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
#include <QTime>
#include <QString>
#include <QGridLayout>
#include <QtConcurrent/QtConcurrent>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    //ui(new Ui::Dialog),
    m_enableTest(false),
    m_seq(1),
    m_cTextEdit(new CTextEdit()),
    m_qTextEdit(new QTextEdit()),
    m_pushButton(new QPushButton(tr("start")))
{
    //ui->setupUi(this);
    auto baseLayout = new QGridLayout();
    baseLayout->addWidget(m_qTextEdit, 0, 0, 1, 2);
    baseLayout->addWidget(m_cTextEdit, 0, 2, 1, 2);
    baseLayout->addWidget(m_pushButton, 1, 2);
    setLayout(baseLayout);

    m_cTextEdit->start();

    connect(m_pushButton, &QPushButton::clicked, this, &Dialog::onButtonClicked);
    connect(this, &Dialog::sendMessage, this, &Dialog::showMessage);

    QMessageBox::information(Q_NULLPTR, tr("thread id"), tr("mainThread: %1").arg(quintptr(QThread::currentThreadId())) );
}

Dialog::~Dialog()
{
    //delete ui;
    clear();
}

void Dialog::testTextEditInThread()
{
    QString data("");
    QString strTid = QString::number(quintptr(QThread::currentThreadId()));

    while (m_enableTest) {
        data = QTime::currentTime().toString(tr(" hh:mm:ss"));
        m_cTextEdit->putData(data);

        data.insert(0, strTid);
        emit sendMessage(data);

        QThread::msleep(10);
    }

    emit sendMessage(tr("%1 stop").arg(strTid));
}

void Dialog::onButtonClicked()
{        
    QFuture<void> f;
    if (m_enableTest) {
        m_enableTest = false;
        f.waitForFinished();
        m_pushButton->setText(tr("start"));
    } else {
        m_enableTest = true;
        f = QtConcurrent::run(this, &Dialog::testTextEditInThread);
        m_pushButton->setText(tr("stop"));
    }
}

void Dialog::showMessage(const QString &data)
{
    QString text(data);
    text.append(tr(" [%1]").arg(m_seq));
    text.append(tr("\n"));

    if (m_seq > 5 || !m_enableTest) {
        m_qTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);  // 位置在最后一行之后
        m_qTextEdit->moveCursor(QTextCursor::Up, QTextCursor::KeepAnchor);  // 所以向上才能选中最后一行
        m_qTextEdit->textCursor().removeSelectedText();
    }

    m_qTextEdit->moveCursor(QTextCursor::Start);
    m_qTextEdit->insertPlainText(text);
    ++m_seq;
}

void Dialog::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e)
    m_enableTest = false;
    m_cTextEdit->putData(tr("end"));
}

void Dialog::clear()
{
    // 等待其他线程退出
    QThread::sleep(1);

    delete m_cTextEdit;
    delete m_qTextEdit;
    delete m_pushButton;
    m_cTextEdit = Q_NULLPTR;
    m_qTextEdit = Q_NULLPTR;
    m_pushButton = Q_NULLPTR;
}
