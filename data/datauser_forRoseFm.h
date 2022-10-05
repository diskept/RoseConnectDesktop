#ifndef DATAUSER_FORROSEFM_H
#define DATAUSER_FORROSEFM_H

#include <QObject>
#include <QJsonObject>

#include "rosefm/rosefm_struct.h"

class DataUser_forRoseFM : public QObject
{
    Q_OBJECT
public:
    explicit DataUser_forRoseFM(QObject *parent = nullptr);
    ~DataUser_forRoseFM();

public:
    void setCountryInfo(const QJsonObject &jsonObj);
    void setChannelInfo(const QJsonObject &jsonObj);

    void getCountryInfo(QList<roseFM::RosefmCountryInfo> &countryInfo);
    void getChannelInfo(QList<roseFM::RosefmChannelInfo> &channelInfo);

private:
    QList<roseFM::RosefmCountryInfo> *list_countryInfo;
    QList<roseFM::RosefmChannelInfo> *list_channelInfo;

};

#endif // DATAUSER_FORROSEFM_H
