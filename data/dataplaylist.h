#ifndef DATAPLAYLIST_H
#define DATAPLAYLIST_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 플레이리스트 데이터 클래스
 * @details API MY 플레이리스트 키값을 변수로 사용, thumbnails 만 변경함.
 */
class DataPlayList : public QObject
{
    Q_OBJECT
public:
    explicit DataPlayList(QObject *parent = nullptr);
    virtual ~DataPlayList();

    void setData(const QJsonObject &p_json);

    QString getAlbum_key() const;

    int getId() const;
    bool getIsFavorite() const;
    bool getIsFavoriteTrack() const;
    QString getMemo() const;
    QString getName() const;
    QString getShare() const;
    int getStar() const;
    QString getTags() const;
    QString getThumb_type() const;
    QString getThumb_url() const;
    int getTotalcount() const;
    QString getType() const;
    QString getUser_name() const;

private :
    QString album_key;
    int id;             ///< 플레이리스트 No (PK)
    bool isFavorite;
    bool isFavoriteTrack;
    QString memo;
    QString name;
    QString share;
    int star;
    QString tags;
    QString thumb_type;
    QString thumb_url;
    int totalcount;
    QString type;
    QString user_name;

};

#endif // DATAPLAYLIST_H
