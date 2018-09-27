#include "serialportthread.h"

#include <QMessageBox>

SerialPortThread::SerialPortThread()    
{
    m_serial = new QSerialPort;
}

void SerialPortThread::setParams(const QString &name, int baud, int dataBits, int stopBits, int parity)
{
    m_serialName = name;
    m_baudRate = baud;
    m_dataBits = dataBits;
    m_stopBits = stopBits;
    m_parity = parity;
}

bool SerialPortThread::openPort()
{
    closePort();

    m_serial->setPortName(m_serialName);
    m_serial->setBaudRate(m_baudRate);
    m_serial->setDataBits((QSerialPort::DataBits)m_dataBits);
    m_serial->setStopBits((QSerialPort::StopBits)m_stopBits);
    m_serial->setParity((QSerialPort::Parity)m_parity);

    if (!m_serial->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, "ERROR", "Serial port for writing open failed!");
        return false;
    }

    return true;
}

void SerialPortThread::closePort()
{
    if (m_serial->isOpen()) {
        m_serial->clear();
        m_serial->clearError();
        m_serial->close();
    }
}

bool SerialPortThread::sendData(const QString &data)
{
    int bytesWritten = m_serial->write(data.toLatin1().data(), data.length());
    if (bytesWritten == data.length())
        return true;

    return false;
}

QString SerialPortThread::receiveData()
{
    QString ret;
    if (!m_queue.Pop(ret, false))
        ret.clear();

    return ret;
}

void SerialPortThread::stop()
{
    requestInterruption();
}

void SerialPortThread::run()
{
    QSerialPort serial;
    serial.setPortName(m_serialName);
    serial.setBaudRate(m_baudRate);
    serial.setDataBits((QSerialPort::DataBits)m_dataBits);
    serial.setStopBits((QSerialPort::StopBits)m_stopBits);
    serial.setParity((QSerialPort::Parity)m_parity);

    if (!serial.open(QIODevice::ReadOnly)) {
        //QMessageBox::critical(nullptr, "ERROR", "Serial port for reading open failed!");
        return;
    }

    while (!isInterruptionRequested()) {
        QString bytesRead;
        if (serial.waitForReadyRead(1000)) {
            bytesRead = serial.readAll();
        }
        if (!bytesRead.isEmpty())
            m_queue.Push(bytesRead);
    }
}
