#ifndef DATAMUSICCOMPOSER_H
#define DATAMUSICCOMPOSER_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 음악-작곡가 데이터 클래스
 * @details DB 쿼리 키값 사용
 */
class DataMusicComposer: public QObject
{
    Q_OBJECT
public:
    explicit DataMusicComposer(QObject *parent = nullptr);
    ~DataMusicComposer();

    void setData(const QJsonObject &p_json);

    int getNumber_of_tracks() const;
    QString getComposer() const;
    void setAlbum_art(const QString &p_album_art);
    QString getAlbum_art() const;

private:
    int number_of_tracks;
    QString composer;
    QString album_art;
};

#endif // DATAMUSICCOMPOSER_H
