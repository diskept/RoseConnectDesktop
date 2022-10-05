#ifndef FINDSMBIP_H
#define FINDSMBIP_H

#include <QObject>
#include <QTcpSocket>
#include <QHostInfo>//c220908_2

/**
 * @brief FindSMBIP::FindSMBIP : SMB 연결 가능 IP 조회 @n
 *      아이피 대역 1~255 중 SMB 연결 가능한 IP를 조회한다. @n
 *      SMB 포트 445만 확인하며, NETBIOS port 139 미확인 @n
 */
class FindSMBIP : public QObject
{
    Q_OBJECT
public:
    explicit FindSMBIP(QObject *parent = nullptr);
    ~FindSMBIP();

    void setIP(const qint64 &p_uuid,const QString &p_ip);

signals:
    void signal_startFind();
    void signal_findedSMBIP(const qint64 &p_uuid, const QString &p_ip, const QString &p_hostName, const bool &flagOn);
    void signal_endedFind();

private:
    qint64 uuid;
    QString ipPrefix;
    int resultCnt;
    QList<QTcpSocket*> list_socket;
    void closeAllSocket();

public slots:
    void findNetworkHost();

private slots:
    void lookedUp(const QHostInfo &host);//c220908_2
    void connectedSocket();
    void errorSocket(QAbstractSocket::SocketError);

};

#endif // FINDSMBIP_H
