#ifndef DATAMUSICARTIST_H
#define DATAMUSICARTIST_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 음악-아티스트 데이터 클래스
 * @details DB 쿼리 키값 사용
 */
class DataMusicArtist : public QObject
{
    Q_OBJECT
public:
    explicit DataMusicArtist(QObject *parent = nullptr);
    ~DataMusicArtist();

    void setData(const QJsonObject &p_json);
    int getArtist_id() const;
    int getNumber_of_tracks() const;
    int getNumber_of_albums() const;
    QString getArtist() const;

    void setAlbum_art(const QString &p_album_art);
    QString getAlbum_art() const;

private:
    int artist_id;
    int number_of_tracks;
    int number_of_albums;
    QString artist;
    QString album_art;
};

#endif // DATAMUSICARTIST_H
