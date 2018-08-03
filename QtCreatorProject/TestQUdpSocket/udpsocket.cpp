#include "udpsocket.h"


UdpSocket::UdpSocket(quint16 port, QObject *parent)
    : QThread(parent)
    , m_localPort(port)
    , m_isRunning(true)
    , m_isRead(true)
    , m_writtenTimes(0)
{
//    m_udpSocket = new QUdpSocket();
//    while (m_localPort < MAX_PORT && !m_udpSocket->bind(QHostAddress::LocalHost, m_localPort))
//        ++m_localPort;
    m_tid = quintptr(QThread::currentThreadId());
}

UdpSocket::~UdpSocket()
{
    stop();
}

bool UdpSocket::setLocalPort(quint16 port)
{
//    if (m_udpSocket->isOpen()) {
//        m_udpSocket->close();
//        stop();
//    }
//    if (m_udpSocket->bind(QHostAddress::LocalHost, port)) {
//        m_localPort = port;
//        start();
//        return true;
//    }

//    return false;

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
    stop();
    m_remoteAddress = address;
    m_remotePort = port;
    start();
}

qint64 UdpSocket::readDatagram(char *data, qint64 maxSize)
{
    QString input;
//    if (!m_inQueue.Pop(input, false))
//        return -1;
    if (!m_inQueue.Pop(input, false)) {
        qDebug() << QObject::tr("[receive*1*:%1] ").arg(m_tid);
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
    m_isRead = false;
    m_writtenTimes.ref();
//    if (m_outQueue.put(QString("%1").arg(data, size)))
//        return size;
    QString sendData = QString("%1").arg(data, size);
    if (m_outQueue.put(sendData)) {
        qDebug() << QObject::tr("[sent*0*:%1] %2").arg(m_tid).arg(sendData);
        return size;
    }

    return -1;
}

void UdpSocket::run()
{
    quintptr tid = quintptr(QThread::currentThreadId());


    QUdpSocket socket;
    while (m_localPort < MAX_PORT && !socket.bind(QHostAddress::LocalHost, m_localPort, QAbstractSocket::ReuseAddressHint)) {
        ++m_localPort;
    }
    if (m_localPort >= MAX_PORT) {
        m_isRunning = false;
        emit error(UdpThreadError::UnavailablePortError);
        return;
    }
qDebug() << QObject::tr("[%1] enter").arg(tid);
    char *data = new char[DATAGRAM_LENGTH]();
    qint64 len = DATAGRAM_LENGTH;
    while (m_isRunning) {
        if (m_isRead) {
            if (socket.hasPendingDatagrams()) { //m_udpSocket
                qint64 curLen = socket.pendingDatagramSize(); // m_udpSocket
                if (curLen > len) {
                    delete[] data;
                    data = new char[curLen+1]();
                    len = curLen + 1;
                }
                curLen = socket.readDatagram(data, curLen, m_remoteAddress, m_remotePort); // m_udpSocket
                if (-1 != curLen) {
                    data[curLen] = '\0';
                    m_inQueue.Push(QString(data));
                    qDebug() << QObject::tr("[receive:%1] %2").arg(tid).arg(data);
                    memset(data, '\0', len);
                }
            }
        } else {
            QString *output;
            qint32 cnt(0);
            while ((cnt = m_outQueue.get(&output)) > 0) {
                for (qint32 idx=0; idx<cnt; ++idx) {
                    if (-1 != socket.writeDatagram(output[idx].toLocal8Bit(), *m_remoteAddress, *m_remotePort)) { // m_udpSocket
                        if (!m_writtenTimes.deref()) {
                            m_isRead = true;
                        }
                        qDebug() << QObject::tr("[sent:%1] %2").arg(tid).arg(output[idx]);
                    }
                }
            }

            static int test3(0);
            if (test3 < 3) {
                qDebug() << QObject::tr("[%1] write over").arg(tid);
                ++test3;
            }
//            m_isRead = true;
        }
    }
    delete[] data;
    data = Q_NULLPTR;
    socket.close();
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
