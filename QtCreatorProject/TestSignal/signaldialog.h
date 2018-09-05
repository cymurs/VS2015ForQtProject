#ifndef SIGNALDIALOG_H
#define SIGNALDIALOG_H

#include <QDialog>

namespace Ui {
class SignalDialog;
}

class SignalDialog : public QDialog
{
    Q_OBJECT

public:
    struct st_Signal{
      int num;
      QString str;
    };

signals:
    void signalStruct(const st_Signal &st, const QString &desc);

public:
    explicit SignalDialog(QWidget *parent = 0);
    ~SignalDialog();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void slotOnStructSignal(const st_Signal &st, const QString &desc);

private:
    Ui::SignalDialog *ui;
};

#endif // SIGNALDIALOG_H
