#ifndef MUSICLIST_COLLECTIONTRACK_H
#define MUSICLIST_COLLECTIONTRACK_H

#include <QWidget>

#include <common/linker.h>
#include <delegate/musiccollectiontrackdelegate.h>
#include <widget/customlistwidget.h>
#include <data/datapopup.h>
#include <music/proc_addtackinplaylist.h>

/**
 * @brief 음악 > My Collection > Track View All @n
 *      legacy_v3/playlist/0 API 사용, 해당 트랙 목록을 DB 조회를 통해 아티스트, 앨범명 정보 확인
 */
class MusicList_CollectionTrack : public QWidget
{
    Q_OBJECT
public:
    explicit MusicList_CollectionTrack(QWidget *parent = nullptr);
    void requestInitData();


private slots:
    void slot_thumbnailDownloaded();
    void slot_delegateClicked(const int &p_index, const int &p_btnType);
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_favoriteTrackChanged();
    void slot_clickedPlayAll();
    void slot_clickedPlayShuffle();

private:

    Linker *linker;

    QList<DataPopup*> *listPopup;

    MusicCollectionTrackDelegate *delegate;
    CustomListWidget *listWidget;

    Proc_AddTackInPlayList *proc_addPlaylist;

    bool flagReqMore = false;
    int totalCount = 0;

    void setInit();
    void setUIControl();

    void requestFavorTracks();
    void playMusic(QJsonArray jsonArrMusic, bool flagPlayShuffle=false);
    QJsonObject getJsonObject_OptionPopup(const int &p_index);
    QJsonObject getPlayMusicInfoObject(const int &p_index);
    QJsonArray getPlayMusicInfoArray(const int &p_index);
    QJsonArray getJsonArr_TrackData(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);
    DataPopup* getTrackInfoFromDB(const QString &p_url);
    void setResultOfFavoriteTrack(const QJsonObject &p_jsonObject);

    void wheelEvent(QWheelEvent *event) override;

};

#endif // MUSICLIST_COLLECTIONTRACK_H
