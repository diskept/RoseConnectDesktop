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
    this->m_server->close();

    this->deleteLater();
}


void HttpServer::start()
{
    //global.device.getDeviceIP()
    this->m_server->listen(QHostAddress::Any, 9284);
}

//
void HttpServer::newConnection()
{
    const auto socket = this->m_server->nextPendingConnection();

    QHostAddress host_address = socket->peerAddress();

    qDebug() << qPrintable(host_address.toString());
    QString tmpIpAddr = host_address.toString().replace("::ffff:", "");

    if(!socket || (tmpIpAddr != global.device.getDeviceIP())){
        //socket->close();
        return;
    }
    else if(socket || (tmpIpAddr == global.device.getDeviceIP())){
        new httpServerConnection(socket);
    }
}
