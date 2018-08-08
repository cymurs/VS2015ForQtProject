#ifndef RIGHTCHILD_H
#define RIGHTCHILD_H

#include <QWidget>

namespace Ui {
class RightChild;
}

class RightChild : public QWidget
{
    Q_OBJECT
public:
    explicit RightChild(QWidget *parent = nullptr);

signals:

public slots:

private:
    Ui::RightChild *ui;
};

#endif // RIGHTCHILD_H
