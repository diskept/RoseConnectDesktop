#ifndef HTTPSERVERCONNECTION_H
#define HTTPSERVERCONNECTION_H

#include "common/linker.h"
#include "widget/dialogconfirm.h"//

#include <memory>
#include <QByteArray>
#include <QMap>
#include <QDialog>
#include <QObject>
#include <QWidget>


class QTcpSocket;

//class httpServerConnection : public QObject
class httpServerConnection : public QWidget
{
    Q_OBJECT

public:
    explicit httpServerConnection(QTcpSocket *socket, QWidget *parent = nullptr);
    ~httpServerConnection() override;

private slots:
    void readyRead();
    void msghide();
    //

signals:

private:

    void setRequestBody(QJsonObject p_jsonObject);
    void msgshow(const QString &p_msgText, int duration);

private:
    Linker *linker;//cheon210617-http

    const std::unique_ptr<QTcpSocket> m_socket;

    struct Request {
        //std::size_t contentLength;//cheon210617-http
        int contentLength;//cheon210617-http

        QString method;
        QString path;
        QString protocol;

        QMap<QString, QString> headers;

        QByteArray body;
    };

    Request m_request;

    enum ConnectionState {
        RequestLine,
        RequestHeaders,
        RequestBody,
        Response
    };

    ConnectionState m_state{ConnectionState::RequestLine};

    QDialog *showMessageDialog;
};

#endif // HTTPSERVERCONNECTION_H
