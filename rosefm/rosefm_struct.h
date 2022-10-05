#ifndef ROSEFM_STRUCT_H
#define ROSEFM_STRUCT_H

#include <QJsonObject>
#include <QString>
#include <list>
#include <QStringList>
#include <QVariantList>

namespace roseFM {

    struct RosefmCountryInfo{
        QString code = "";
        QString name = "";

        int id = 0;
        int sort = 0;

        double fre_min = 0.0;
        double fre_max = 0.0;
    };

    struct RosefmChannelInfo{
        QString type = "";
        QString frequency = "";
        QString imgUrl = "";
        QString retryUrl = "";
        QString key = "";
        QString title = "";
        QString url = "";

        int id = 0;
        int sort = 0;
        int fileId = 0;
    };
}

#endif // ROSEFM_STRUCT_H
