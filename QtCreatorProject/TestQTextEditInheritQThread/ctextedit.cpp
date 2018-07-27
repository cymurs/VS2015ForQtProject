#include "ctextedit.h"
#include <QMessageBox>

CTextEdit::CTextEdit()
 : QTextEdit()
 , QThread()
{

}

void CTextEdit::putData(const QString &data)
{
    queue.Push(data);
}

void CTextEdit::run()
{
    quint32 seq(1);
    QString data;
    QString strTid = QString::number(quintptr(QThread::currentThreadId()));
    strTid.append("  ");

    try {
        while (queue.Pop(data)) {
            if (0 == data.compare(tr("end"))) {
    //            QMessageBox::information(Q_NULLPTR, tr("CTextEdit Thread Stop"), tr("%1 stop.").arg(strTid));
                break;
            }

            if (seq > 5) {
                moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);  // 位置在最后一行之后
                moveCursor(QTextCursor::Up, QTextCursor::KeepAnchor);  // 所以向上才能选中最后一行
                textCursor().removeSelectedText();
            }

            moveCursor(QTextCursor::Start);
            data.insert(0, strTid);
            data.append(tr(" [%1]").arg(seq));
            data.append(tr("\n"));
            insertPlainText(data);
            ++seq;
        }
    } catch (std::exception &e) {

    }
}
