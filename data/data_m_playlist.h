#ifndef DATA_M_PLAYLIST_H
#define DATA_M_PLAYLIST_H

#include <QObject>
#include <QJsonObject>
#include <QList>
#include <QJsonArray>

/**
 * @brief 음악 플레이리스트 데이터 클래스 @n
 *      플레이리스의 트랙 정보를 struct로 관리
 */
class Data_M_PlayList : public QObject
{
    Q_OBJECT
public:
    // 트렉 정보
    struct sDATA_M_Track {
        QString album = "";
        QString artist = "";
        QString client_id = "";
        QString comment = "";
        QString mac_address = "";
        int duration;
        bool isFavorite = false;
        int id = 0;
        int ownerId = 0;
        QString ownerName = "";
        int playCount = 0;
        QString file_url = "";
        int playlistId = 0;
        int sort = 0;
        int star = 0;
        QString thumb_url = "";
        QString title = "";
        QString type = "";
    };
    explicit Data_M_PlayList(QObject *parent = nullptr);
    void setData(const QJsonObject &p_json);
    void initData();
    QJsonArray getTrackJsonArrayOfIndex(int p_index);
    QJsonObject getJsonDataTrackForOptionPopup(int p_index);
    QJsonObject getData();


    int getNo() { return this->id; }        ///< 플레이리스트 PK
    QString getUsername() { return this->username; }
    QString getTitle(){ return this->name; }
    QString getMemo() { return this->memo; }
    QString getToshare() { return this->share; }
    QString getConverimg();
    int getStar() { return this->star; }
    void setStar(int p_cnt){ this->star = p_cnt; }
    bool getFlagPlayListMy(){ return this->flagPlayListMy; }
    bool getFavorites() { return this->isFavorite; }
    void setFavorites(bool p_isFavorite){ this->isFavorite = p_isFavorite; }
    QList<QString> getTags() { return this->tags; }
    QList<sDATA_M_Track> getTrackList() { return this->list_dataTracks; }
    void setFavoritesTrack(int p_index, bool p_isFavorite){ this->list_dataTracks[p_index].isFavorite = p_isFavorite; }

signals:

private:
    bool flagPlayListMy = true; ///< 마이 플레이리스트인지 친구 플레이리스트인지 여부
    QString type = "";
    QString username = "";
    QString name = "";          ///< 플레이리스트명
    QString memo = "";          ///< 설명
    QString thumnail = "";
    QString albumKey = "";
    QString registDateTime = "";
    int id = 0;                 ///< 플레이리스트 PK
    int ownerId = 0;
    int sort = 0;
    int thumbupCount = 0;
    int trackCount = 0;
    bool isRose = false;
    bool isFavorite = false;
    bool isFavoriteTrack = false;
    bool thumbup = false;
    QString share = "";         ///< 0:비공개, 1: 전체공개, 2: 친구공개
    int star = 0;
    QList<QString> tags;        ///< 태그 리스트
    QList<sDATA_M_Track> list_dataTracks;   ///< 트렉 정보의 리스트

    QJsonObject raw_jsonData;                   ///< [raw data] 통 전체 데이터
    QJsonArray raw_arr_thumbs;                  ///< [raw data] 통 썸네일 데이터
    QJsonArray raw_arr_tracks;                  ///< [raw data] 통 전체 트렉 데이터
};

#endif // DATA_M_PLAYLIST_H
