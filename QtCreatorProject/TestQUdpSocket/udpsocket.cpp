#include "udpsocket.h"


UdpSocket::UdpSocket(quint16 port, QObject *parent)
    : QThread(parent)
    , m_localPort(port)
    , m_isRunning(true)
    , m_isRead(true)
{
    m_udpSocket = new QUdpSocket(this);
    while (m_localPort < MAX_PORT && !m_udpSocket->bind(QHostAddress::LocalHost, m_localPort))
        ++m_localPort;
}

UdpSocket::~UdpSocket()
{
    stop();
}

bool UdpSocket::setLocalPort(quint16 port)
{
    if (m_udpSocket->isOpen()) {
        m_udpSocket->close();
        stop();
    }
    if (m_udpSocket->bind(QHostAddress::LocalHost, port)) {
        m_localPort = port;
        start();
        return true;
    }

    return false;
}

quint16 UdpSocket::localPort() const
{
    return m_localPort;
}

void UdpSocket::setRemoteConnection(QHostAddress *address, quint16 *port)
{
    stop();
    m_remoteAddress = address;
    m_remotePort = port;
    start();
}

qint64 UdpSocket::readDatagram(char *data, qint64 maxSize)
{
    QString input;
    if (!m_inQueue.Pop(input, false))
        return -1;

    qint64 retLen = input.length();
    if (retLen > maxSize)
        retLen = maxSize;
    memcpy(data, input.toStdString().c_str(), retLen);
    return retLen;
}

qint64 UdpSocket::writeDatagram(const char *data, qint64 size)
{
    m_isRead = false;
    if (m_outQueue.put(QString("%1").arg(data, size)))
        return size;

    return -1;
}

void UdpSocket::run()
{
    char *data = new char[DATAGRAM_LENGTH]();
    qint64 len = DATAGRAM_LENGTH;
    while (m_isRunning) {
        if (m_isRead) {
            if (m_udpSocket->hasPendingDatagrams()) {
                qint64 curLen = m_udpSocket->pendingDatagramSize();
                if (curLen > len) {
                    delete[] data;
                    data = new char[curLen+1]();
                    len = curLen + 1;
                }
                curLen = m_udpSocket->readDatagram(data, curLen, m_remoteAddress, m_remotePort);
                if (-1 != curLen) {
                    data[curLen] = '\0';
                    m_inQueue.Push(QString(data));
                    memset(data, '\0', len);
                }
            }
        } else {
            QString *output;
            qint32 cnt(0);
            while ((cnt = m_outQueue.get(&output)) > 0) {
                for (qint32 idx=0; idx<cnt; ++idx) {
                    m_udpSocket->writeDatagram(output[idx].toLocal8Bit(), *m_remoteAddress, *m_remotePort);
                }
            }

            m_isRead = true;
        }
    }
    delete[] data;
    data = Q_NULLPTR;
}

void UdpSocket::start(QThread::Priority prior)
{
    m_isRunning = true;
    QThread::start(prior);
}

void UdpSocket::stop()
{
    m_isRunning = false;
}
