#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QThread>
#include <QUdpSocket>
#include "qdoublebufferedqueue.h"
#include "concurrentqueue.h"

#define MAX_PORT 65535
#define DATAGRAM_LENGTH 4096

class UdpSocket : public QThread
{
    Q_OBJECT

public:
    explicit UdpSocket(quint16 port, QObject *parent = Q_NULLPTR);
    virtual ~UdpSocket();

    bool setLocalPort(quint16 port);
    quint16 localPort() const;
    void setRemoteConnection(QHostAddress *address, quint16 *port);
    qint64 readDatagram(char *data, qint64 maxSize);
    qint64 writeDatagram(const char *data, qint64 size);

protected:
    void run() override;
    void start(Priority prior= InheritPriority);
    void stop();

private:
    ConcurrentQueue<QString> m_inQueue;
    QDoubleBufferedQueue<QString> m_outQueue;
    QUdpSocket *m_udpSocket;
    QHostAddress *m_remoteAddress;
    quint16 *m_remotePort;
    quint16 m_localPort;
    bool m_isRunning;
    bool m_isRead;
};

#endif // UDPSOCKET_H
