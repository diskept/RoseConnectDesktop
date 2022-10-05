#ifndef ROSERADIO_STRUCT_H
#define ROSERADIO_STRUCT_H

#include <list>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>


namespace roseRadio {

    struct RoseRadioGenreInfo{
        int id = 0;
        int sort = 0;

        QString name = "";
        QString imageUrl = "";
    };

    struct RoseRadioRegionInfo{
        int id = 0;
        int sort = 0;

        QString name = "";
    };

    struct RoseRadioLanguageInfo{
        int id = 0;
        int sort = 0;

        QString name = "";
    };
}
#endif // ROSERADIO_STRUCT_H
