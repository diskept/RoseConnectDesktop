#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QTcpServer>

#include <memory>

class QTcpServer;
class HttpServer :

public QThread//c220716
//public QObject//c220716
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer() override;

    void start();

private slots:
    void slot_httpServer_start();//c230303
    void newConnection();
    //void slot_toatmsgpop();

private:
    const std::unique_ptr<QTcpServer> m_server;
};
#endif // HTTPSERVER_H
