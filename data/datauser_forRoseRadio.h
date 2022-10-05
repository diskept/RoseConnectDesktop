#ifndef DATAUSER_FORROSERADIO_H
#define DATAUSER_FORROSERADIO_H

#include "roseRadio/roseradio_struct.h"
#include "cdplay/CDplay_struct.h"//c220721

#include <QObject>
#include <QJsonObject>


class DataUser_forRoseRadio : public QObject
{
    Q_OBJECT
public:
    explicit DataUser_forRoseRadio(QObject *parent = nullptr);
    ~DataUser_forRoseRadio();

public:
    void setGenreInfo(const QJsonArray&);
    void setRegionInfo(const QJsonArray&);
    void setLanguageInfo(const QJsonArray&);

    QList<roseRadio::RoseRadioGenreInfo>* getGenreInfo();
    QList<roseRadio::RoseRadioRegionInfo>* getRegionInfo();
    QList<roseRadio::RoseRadioLanguageInfo>* getLanguageInfo();

private:
    QList<roseRadio::RoseRadioGenreInfo> *list_genreInfo;
    QList<roseRadio::RoseRadioRegionInfo> *list_regionInfo;
    QList<roseRadio::RoseRadioLanguageInfo> *list_languageInfo;
};
#endif // DATAUSER_FORROSERADIO_H
