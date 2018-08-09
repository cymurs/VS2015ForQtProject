#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class TopChild;
class LeftChild;
class RightChild;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;


private:
    Ui::Widget *ui;
    TopChild *m_top;
    LeftChild *m_left;
    RightChild *m_right;
};

#endif // WIDGET_H
