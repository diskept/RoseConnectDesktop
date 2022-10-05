#ifndef MUSICCOLLECTION_H
#define MUSICCOLLECTION_H

#include "common/linker.h"
#include "proc_addtackinplaylist.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <delegate/musiccollectiontrackdelegate.h>
#include <data/datapopup.h>
#include <widget/VerticalScrollArea.h>

/**
 * @brief 음악 > My collection @n
 *      사용자가 즐겨찾기한 트랙, My Playlist, Friend Playlist 모음@n
 *      각 항목별 최소 갯수만 Show, 각 항목 View All 클릭시 전체 확인 가능
 *
 * @note
 *      My Playlist @sa FrameMyPlayList 로 목록 구성 @n
 *      Friend Playlist @sa FrameFriendPlayList 로 목록 구성
 */
class MusicCollection : public QWidget
{
    Q_OBJECT
public:
    explicit MusicCollection(QWidget *parent = nullptr);
    ~MusicCollection();

    void requestFlagLoginInfo();

signals:
    void signal_clickedViewAll(QString p_viewAllMode);

public slots:
    void slot_requestData();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedViewAll();
    void slot_thumbnailDownloaded();
    void slot_delegateClicked(const int &p_index, const int &p_btnType);
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);
    void slot_myPlaylistChanged();
    void slot_favoriteTrackChanged();
    void slot_clickedLogin();

private :

    Linker *linker;

    QVBoxLayout *boxContents;
    QList<DataPopup*> *listPopup;
    QWidget *widgetLogin;
    VerticalScrollArea *scrollArea_content;


    Proc_AddTackInPlayList *proc_addPlaylist;

    // Favor Track
    QLabel *lb_emptyFavorTrack;
    MusicCollectionTrackDelegate *delegateTracks;
    QListWidget *listWidget;
    QWidget *widgetTracks;

    // MY 플레이리스트
    QLabel *lb_emptyMyPlayList;
    QHBoxLayout *boxMyPlayList;
    QStackedWidget *stackMyPlayList;

    // 친구 플레이리스트
    QLabel *lb_emptyFriendPlayList;
    QHBoxLayout *boxFriendPlayList;
    QStackedWidget *stackFriendPlayList;

    // 데이터 갱신 uuid : currentMSec 이용
    qint64 uuid;

    void setInit();
    void setUIControl();
    void setLoginInfoUI();
    void setFavorTrackListUI();
    void setMyPlayListUI();
    void setFriendPlayListUI();
    void setResultOfFavoriteTrack(const QJsonObject &p_jsonObject);
    void setResultOfMyPlayList(const QJsonObject &p_jsonObject);
    void setResultOfFrienPlayList(const QJsonObject &p_jsonObject);
    void appendMyPlayList(const QJsonObject &p_jsonObject);
    void appendFriendPlayList(const QJsonObject & p_jsonObect);
    void requestFavorTracks();
    void requestMyPlayList();
    void requestFriendPlayList();
    void requestShowOptionPopup(const int &p_index);
    DataPopup* getTrackInfoFromDB(const QString &p_url);
    QJsonObject getJsonObject_OptionPopup(const int &p_index);
    QJsonArray getJsonArr_TrackData(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);

};

#endif // MUSICCOLLECTION_H
