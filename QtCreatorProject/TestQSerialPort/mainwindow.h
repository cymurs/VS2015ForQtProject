#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>
#include <QGridLayout>
#include <QGroupBox>
#include <QtCharts>
#include "qdoublebufferedqueue.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void onButtonClicked();
    void onTimerTimeout();
    void onSeriesChanged();

private:
    void Initialize();
    void connectSlots();
    void serialPortReadThread();
    void randomPoint4ChartThread();

private:
    Ui::MainWindow *ui;
    bool isOpened;
    bool isGenerateRand;
    QSerialPort *m_serialPort;
    QTimer m_showTimer;
    QDoubleBufferedQueue<QString> m_dbQueue;
    QSplineSeries *m_splineSeries;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    QTimer m_seriesTimer;
};

#endif // MAINWINDOW_H
