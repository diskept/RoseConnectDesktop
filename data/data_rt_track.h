#ifndef DATA_RT_TRACK_H
#define DATA_RT_TRACK_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 로즈 트랙 데이터 클래스
 * @details API 트랙 키값을 변수로 사용.
 */
class Data_RT_Track : public QObject
{
    Q_OBJECT
public:
    explicit Data_RT_Track(QObject *parent = nullptr);
    ~Data_RT_Track();

    void setDataFromCrawling(const QJsonObject &p_json);
    void setData(const QJsonObject &p_json);
    QJsonObject getDataJson();

    int getNo() const;
    int getPlaylistno() const;
    int getStar() const;
    QString getTitle() const;
    QString getArtist() const;
    QString getPlayurl() const;
    QString getChannelId() const;
    QString getThumbnail() const;
    QString getChannelTitle() const;
    QString getDuration() const;
    QString getViewCount() const;
    QString getConvertDuration() const;
    QString getFavorites() const;

    void setFavorites(const QString &value);

private:
    int no = 0;
    int star = 0;
    int playlistno = 0;
    QString title;
    QString artist;
    QString playurl;
    QString channelId;
    QString thumbnail;
    QString channelTitle;
    QString duration;           // ex) PT3M19S
    QString viewCount;
    QString favorites;
    QString clientKey;
    QString comment;
    QString categoryId;
    QString description;
    QString definition;
    QString dimension;
    QString likeCount;

};

#endif // DATA_RT_TRACK_H
