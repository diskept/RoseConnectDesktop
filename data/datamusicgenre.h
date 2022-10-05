#ifndef DATAMUSICGENRE_H
#define DATAMUSICGENRE_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 음악-장르 데이터 클래스
 * @details DB 쿼리 키값 사용
 */
class DataMusicGenre : public QObject
{
    Q_OBJECT
public:
    explicit DataMusicGenre(QObject *parent = nullptr);
    ~DataMusicGenre();

    void setData(const QJsonObject &p_json);
    int getGenre_id() const;
    int getNumber_of_album() const;
    QString getName() const;
    void setAlbum_art(const QString &p_album_art);
    QString getAlbum_art() const;

private:
    int genre_id;
    int numalbums;
    QString name;
    QString album_art;
};

#endif // DATAMUSICGENRE_H
