#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <QWidget>

class ChildWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ChildWindow(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // CHILDWINDOW_H