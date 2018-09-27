#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H

#include <QThread>
#include <QtSerialPort/QSerialPort>
#include "concurrentqueue.h"

class SerialPortThread : public QThread
{
    Q_OBJECT
public:
    SerialPortThread();

    void setParams(const QString &name, int baud, int dataBits, int stopBits, int parity);
    bool openPort();
    void closePort();
    bool sendData(const QString &data);
    QString receiveData();
    void stop();

protected:
    void run() override;

signals:

public slots:

private:
    QSerialPort *m_serial;
    QString m_serialName;
    int m_baudRate;
    int m_dataBits;
    int m_stopBits;
    int m_parity;
    ConcurrentQueue<QString> m_queue;
};

#endif // SERIALPORTTHREAD_H
