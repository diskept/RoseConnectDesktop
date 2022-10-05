#ifndef FINDIP_H
#define FINDIP_H

#include <QString>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHostInfo>

/**
 * @brief The FindIP class : 내 아이피 확인 @n
 * 2개의 함수로 구성 내 아이피 확인 @sa FindIP::getMyIP(), 내 맥주소 확인 @sa FindIP::getMacAddress()
 */
class FindIP
{
public:
    FindIP();
    ~FindIP();
    QString getMyIP();
    QString getMacAddress(QString ip);
};

#endif // FINDIP_H
