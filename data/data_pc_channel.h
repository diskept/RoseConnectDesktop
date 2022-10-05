#ifndef DATA_PC_CHANNEL_H
#define DATA_PC_CHANNEL_H

#include <QObject>
#include <QJsonObject>

/**
 * @brief 팟캐스트 채널 목록 데이터 Object @n
 *      API 팟캐스트 인기 채널 목록의 키값을 변수로 사용
 * @note collectionId 값의 경우 API 별로 type 다름. @n
 * 팟캐스트-인기 채널 목록 의 경우 int형, 팟캐스트-구독 채널 목록의 경우 String형 @n
 */
class Data_PC_Channel : public QObject
{
    Q_OBJECT
public:
    explicit Data_PC_Channel(QObject *parent = nullptr);
    explicit Data_PC_Channel(const QJsonObject &p_json, QObject *parent = nullptr);

    void setData(const QJsonObject &p_json);
    QJsonObject getData() ;

    QString getArtistName() const;

    QString getCollectionName() const;

    QString getFeedUrl() const;

    QString getArtworkUrl100() const;

    QString getArtworkUrl600() const;

    QString getCollectionViewUrl() const;

    QString getCollectionId() const;

    bool getIsInit() const;

    bool getIsSubScribe() const;

private:
    QString artistName;
    QString collectionName;
    QString feedUrl;
    QString artworkUrl100;
    QString artworkUrl600;
    QString collectionViewUrl;
    QString collectionId;

    // 구독채널 전용
    bool isInit=false;
    bool isSubScribe=false;
};

#endif // DATA_PC_CHANNEL_H
