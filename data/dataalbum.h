#ifndef DATAALBUM_H
#define DATAALBUM_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 앨범 데이터 클래스
 * @details API 최근앨범목록의 키값을 변수로 사용
 */
class DataAlbum : public QObject
{
    Q_OBJECT
public:
    explicit DataAlbum(QObject *parent = nullptr);
    ~DataAlbum();

    void setData(const QJsonObject &p_json);
    QJsonObject getDataTotal();

    QString getAlbum() const;

    void setAlbum_art(const QString &p_album_art);
    QString getAlbum_art() const;
    int getAlbum_id() const;
    QString getAlbum_key() const;
    QString getArtist() const;
    int getArtist_id() const;
    QString getArtist_key() const;
    QString getComposer() const;
    int getComposercnt() const;
    QString getData() const;
    QString getDuration() const;
    int getGener_song_cnt() const;
    QString getId() const;
    bool getIsSelected() const;
    int getNum_of_song() const;
    int getNum_of_tracks() const;
    QString getTitle() const;
    QString getTrack_id() const;
    int getFavorite() const;
    QString getSection() const;
    QString getMimeType() const;
    int getSamplerate() const;
    int getBitdepth() const;

private:
    QString album;
    QString album_art;
    int album_id;
    QString album_key;
    QString artist;
    int artist_id;
    QString artist_key;
    QString composer;
    int composercnt;
    QString data;
    QString duration;
    int gener_song_cnt;
    QString id;
    bool isSelected;
    int num_of_song;
    int num_of_tracks;
    QString title;
    QString track_id;
    int favorite=0;
    QString mime_type;
    int sample_rate = 0;
    int bit_depth = 0;
    QString section;
};

#endif // DATAALBUM_H
