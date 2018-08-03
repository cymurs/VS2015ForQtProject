#include "udpsocket.h"


UdpSocket::UdpSocket(quint16 port, QObject *parent)
    : QThread(parent), m_localPort(port)
{
    m_sendSocket = new QUdpSocket();
    m_tid = quintptr(QThread::currentThreadId());
}

UdpSocket::~UdpSocket()
{
    if (m_sendSocket) {
        delete m_sendSocket;
        m_sendSocket = Q_NULLPTR;
    }
    stop();
}

bool UdpSocket::setLocalPort(quint16 port)
{
    stop();
    m_localPort = port;
    start();
    return true;
}

quint16 UdpSocket::localPort() const
{
    return m_localPort;
}

void UdpSocket::setRemoteConnection(QHostAddress *address, quint16 *port)
{
    {
        QMutexLocker locker(&m_remoteMutex);
        m_remoteAddress = *address;
        m_remotePort = *port;
    }
    if (!isRunning())
        start();
}

qint64 UdpSocket::readDatagram(char *data, qint64 maxSize)
{
    QString input;
    if (!m_inQueue.Pop(input, false)) {
//        qDebug() << QObject::tr("[dest no:%1] ").arg(m_tid);
        return -1;
    }

    qint64 retLen = input.length();
    if (retLen > maxSize)
        retLen = maxSize;
    memcpy(data, input.toStdString().c_str(), retLen);
    return retLen;
}

qint64 UdpSocket::writeDatagram(const char *data, qint64 size)
{
    return m_sendSocket->writeDatagram(data, size, m_remoteAddress, m_remotePort);
}

void UdpSocket::run()
{
    quintptr tid = quintptr(QThread::currentThreadId());
    qDebug() << QObject::tr("[%1] enter[port:%2]").arg(tid).arg(m_localPort);

    QUdpSocket socket;
    char *data = new char[DATAGRAM_LENGTH]();
    qint64 len = DATAGRAM_LENGTH;
    while (!isInterruptionRequested()) {
        if (QAbstractSocket::UnconnectedState == socket.state()) {
            while (m_localPort < MAX_PORT && !socket.bind(QHostAddress::LocalHost, m_localPort, QAbstractSocket::ReuseAddressHint)) {
                ++m_localPort;
            }
            if (m_localPort >= MAX_PORT) {
                emit error(UdpThreadError::UnavailablePortError);
                return;
            }
        }

        if (socket.hasPendingDatagrams()) {
            qint64 curLen = socket.pendingDatagramSize();
            if (curLen > len) {
                delete[] data;
                data = new char[curLen+1]();
                len = curLen + 1;
            }
            {
                QMutexLocker locker(&m_remoteMutex);
                curLen = socket.readDatagram(data, curLen, &m_remoteAddress, &m_remotePort);
            }
            if (-1 != curLen) {
                data[curLen] = '\0';
                m_inQueue.Push(QString(data));
                qDebug() << QObject::tr("[receive:%1] %2").arg(tid).arg(data);
                memset(data, '\0', len);
            }
        }
    }
    delete[] data;
    data = Q_NULLPTR;
    socket.close();
}

void UdpSocket::stop()
{
    requestInterruption();
}
