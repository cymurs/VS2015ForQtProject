#ifndef CTEXTEDIT_H
#define CTEXTEDIT_H

#include <QTextEdit>
#include <QThread>
#include "concurrentqueue.h"


class CTextEdit : public QTextEdit, public QThread
{
    Q_OBJECT
public:
    CTextEdit();

    void putData(const QString &data);
signals:

public slots:

protected:
    void run();

private:
    ConcurrentQueue<QString> queue;   
};

#endif // CTEXTEDIT_H
