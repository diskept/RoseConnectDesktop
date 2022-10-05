#include "httpServer.h"
#include "common/global.h"
#include "common/httpServerConnection.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

HttpServer::HttpServer(QObject *parent) :
    QThread(parent),//c220716
    //QObject(parent),//c220716
    m_server(std::make_unique<QTcpServer>())
{
    connect(m_server.get(), &QTcpServer::newConnection, this, &HttpServer::newConnection);
}


HttpServer::~HttpServer()//= default;
{
    print_debug();
    this->quit();//c220716
}


void HttpServer::start()
{
    //global.device.getDeviceIP()
    m_server->listen(QHostAddress::Any, 9284);
}

//
void HttpServer::newConnection()
{
    const auto socket = m_server->nextPendingConnection();

    QHostAddress host_address = socket->peerAddress();

    qDebug() << qPrintable(host_address.toString());
    QString tmpIpAddr = host_address.toString().replace("::ffff:", "");

    if(!socket || (tmpIpAddr != global.device.getDeviceIP())){
        socket->deleteLater();
        return;
    }

    new httpServerConnection(socket);
}
