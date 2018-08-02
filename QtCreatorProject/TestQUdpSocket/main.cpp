#include <QCoreApplication>
#include <QDebug>
#include <thread>
#include "udpsocket.h"
#include "helper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::thread th = std::thread([](){
        QHostAddress address("127.0.0.1");
        quint16 port(3478);
        UdpSocket send(5566);
        send.setRemoteConnection(&address, &port);
        const char *sentArray[] = {
            "Number display: 112233445566",
            "Letter display: abcdefgh",
            "Other display: *#*#$%^&#*#*"
        };
        int sizeArray[] = {28, 24, 27};
        quintptr tid = quintptr(QThread::currentThreadId());
        for (int i=0; i<100; ++i) {
            int idx = i % 3;
            QString sentData(QObject::tr("%1---%2").arg(i+1).arg(sentArray[idx]));
            send.writeDatagram(qPrintable(sentData), sentData.length());
            qDebug() << QObject::tr("[sent%1] %2").arg(tid).arg(sentData);

            QThread::msleep(100);
        }
    });
    th.detach();

    QHostAddress address("127.0.0.1");
    quint16 port(5566);
    UdpSocket recv(3478);
    recv.setRemoteConnection(&address, &port);

    quintptr tid = quintptr(QThread::currentThreadId());
    char *recData = new char[512]();
    qint64 recvSize(0);
    for (int i=0; i<20; ++i) {
        while(-1 != (recvSize = recv.readDatagram(recData, 512))) {
            qDebug() << QObject::tr("[%1] %2").arg(tid).arg(recData);
            memset(recData, '\0', 512);
        }
        QThread::msleep(100);
        //qDebug() << "==receive data jump==";
    }

    return a.exec();
}
