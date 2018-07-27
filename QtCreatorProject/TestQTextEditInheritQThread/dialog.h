#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "ctextedit.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

Q_SIGNALS:
    void update(qint64, const QString &);
    void threadWidget();
    void sendMessage(const QString &);

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void testTextEditInThread();
public Q_SLOTS:
    void onButtonClicked();
    void showMessage(const QString &);

protected:
    void closeEvent(QCloseEvent *e);

private:
    void clear();

private:
    //Ui::Dialog *ui;
    bool m_enableTest;
    quint32 m_seq;
    CTextEdit *m_cTextEdit;
    QTextEdit *m_qTextEdit;
    QPushButton *m_pushButton;
};

#endif // DIALOG_H
