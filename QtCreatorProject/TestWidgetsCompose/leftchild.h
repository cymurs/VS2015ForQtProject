#ifndef LEFTCHILD_H
#define LEFTCHILD_H

#pragma execution_character_set("utf-8")  // 解决界面显示中文乱码
#include <QWidget>
#include <QPushButton>


namespace Ui {
class LeftChild;
}

class LeftChild : public QWidget
{
    Q_OBJECT
public:
    explicit LeftChild(QWidget *parent = nullptr);
//    void setButtonSize(int w, int h);

signals:

public slots:

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void changeStyleSheet(QPushButton *btn);

private:
    Ui::LeftChild *ui;
    QPushButton *m_timeSrc;
    QPushButton *m_serialPort;
    QPushButton *m_network;
    QPushButton *m_state;
    QPushButton *m_curPressed;
};


#endif // LEFTCHILD_H
