#ifndef CDPLAY_STRUCT_H
#define CDPLAY_STRUCT_H

#include <list>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>


namespace CDplay {

    struct CDItemData {//c220724

        QString title = "";
        QString artist;

        QString imageUrl = "";

        QString info = "";
        QString isRipping = "";
        QString recording = "";

        int numberOfTracks = 0;
        int playPosition = 0;
    };
}
#endif // CDPLAY_STRUCT_H
