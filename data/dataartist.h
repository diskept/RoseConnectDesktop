#ifndef DATAARTIST_H
#define DATAARTIST_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 아티스트 데이터 클래스
 * @details API 추천아티스트 목록의 키값을 변수로 사용
 */
class DataArtist : public QObject
{
    Q_OBJECT
public:
    explicit DataArtist(QObject *parent = nullptr);

    void setData(const QJsonObject &p_jsonData);

    QString getArtist() const;
    int getId() const;
    QString getHead() const;
    int getNumber_of_albums() const;
    int getNumber_of_tracks() const;
    QString getAlbum_art() const;


private:
    QString artist;
    int _id;
    QString head;
    int number_of_albums;
    int number_of_tracks;
    QString album_art;
};

#endif // DATAARTIST_H
