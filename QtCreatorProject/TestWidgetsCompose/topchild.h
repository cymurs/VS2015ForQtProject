#ifndef TOPCHILD_H
#define TOPCHILD_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class TopChild;
}

class TopChild : public QWidget
{
    Q_OBJECT
public:
    explicit TopChild(QWidget *parent = nullptr);


signals:

public slots:


private:
    Ui::TopChild *ui;
    QPushButton *m_homeBtn;
    QPushButton *m_backBtn;
};

#endif // TOPCHILD_H
