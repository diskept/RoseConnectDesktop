#ifndef MUSICE_PLAYLIST_H
#define MUSICE_PLAYLIST_H

#include "home/abstractpage.h"
#include "common/linker.h"
#include "data/data_rt_playlist.h"
#include "data/data_rt_track.h"
#include "widget/clickablelabel.h"
#include "widget/imagelabel.h"
#include "widget/flowlayout.h"
#include "QLabel"
#include "QListWidget"
#include "QPushButton"
#include "widget/flowlayout.h"
#include "login/dialog_playlist.h"

#include <data/data_m_playlist.h>

#include <delegate/musicdelegate.h>

/**
 * @brief 음악 플레이리스트 상세 @n
 *      My Playlist, Friend Playlist 공통 사용
 */
class Musice_playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Musice_playlist(QWidget *parent = nullptr);
    void setPlayListData(const QJsonObject &p_jsonObject);

signals:
    void signal_changedMenuName(const QString &p_menuName);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData);
    void slot_delegateClicked(const int &p_index,const int &p_btnType);
    void slot_playlistClicked(const int &p_playlistno);
    void slot_thumbnailDownloaded();
    void slot_clickedStar();
    void slot_clickedHeart();
    void slot_clickedMore();
    void slot_clickedPlay();
    void slot_clickedShuffle();
    void slot_myPlaylistTrackChanged();

private:

    Linker *linker;
    MusicDelegate *delegate;

    int playlistNo;
    int cntStarTmp;

    QPixmap pixmap_heart_on;
    QPixmap pixmap_heart_off;

    QLabel *lb_writer;
    ImageLabel *lb_image;
    QLabel *lb_title;
    QLabel *lb_memo;
    QPushButton *btn_heart;
    QListWidget *listWidget;
    QList<ClickableLabel*> *list_lb_star;
    FlowLayout *flowLayout_tag;
    QWidget *widget_star;

    Data_M_PlayList *dataPlayList;
    //QList<Data_RT_Track*> *list_data_RT_Track;
    QString type = "";
    QString thumbnail_type = "";
    QJsonObject playlist_obj;

    Dialog::Dialog_Playlist *dlg_playList;

    void setInit();
    void setUIControl();
    void setFavoriteUI();
    void setStarPointUI();
    void setTagListUI();
    void changeTrackFavoritesUI(const int &p_index);
    void requestPlaysListFetch();
    void requestAddPlaylistTrack(const int &p_playlistno, const QString &p_playurl);
    void requestNewPlaylistTrack(const QString &p_name, const QString &p_playurl);
    void requestUpdateTrackFavorites(const int &p_index);
    void requestPlay(QJsonArray jsonPlayList, bool flagShuffle);

    void setResultOfStar(const QJsonObject &p_jsonObject);
    void setResultOfFavorites(const QJsonObject &p_jsonObject);
    void setResultOfPlayListFetch(const QJsonObject &p_jsonObject);
    void setResultOfAddPlaylistTrack(const QJsonObject &p_jsonObject);
    void setResultOfAddNewPlaylist(const QJsonObject &p_jsonObject);
    void setResultOfUpdateTrackFavorites(const int &p_index, const QJsonObject &p_jsonObject);

    void appendTrackUI(const int &p_index);
    QJsonArray getPlayList(int p_index = -1);

};

#endif // MUSICE_PLAYLIST_H
