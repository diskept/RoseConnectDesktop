#ifndef PROC_ADDROSETUBEINPLAYLIST_H
#define PROC_ADDROSETUBEINPLAYLIST_H

#include <QWidget>
#include "common/linker.h"
#include "login/dialog_playlist.h"
#include "login/dialog_newplaylist.h"

/**
 * @brief 로즈튜브 플레이리스트 담기 Proc @n
 *      @sa Dialog::Dialog_Playlist 를 이용해 플레이리스트 선택 후 담기 지원 @n
 */
class Proc_AddRosetubeInPlayList : public QWidget
{
    Q_OBJECT
public:
    explicit Proc_AddRosetubeInPlayList(QWidget *parent = nullptr);
    void setPlayUrl(QString p_playUrl);
    void showDlgOfPlayList();
    void showDlgOfNewPlayList();
    void requestAddVideoInPlaylist(const int &p_playlistno);
    void requestAddVideoInPlaylist(const int &p_playlistno, const QString &p_playurl);
    void requestNewVideoInPlaylist(const QString &p_name);
    void requestNewVideoInPlaylist(const QString &p_name, const QString p_playurl);

signals :
    void signal_playlistClicked(const int &p_playlistno);
    void signal_newPlaylistNameEdited(QString p_name);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private :
    Linker *linker;
    Dialog::Dialog_Playlist *dlg_playList;

    QString playUrl;

    void setInit();
    void setUIControl();
    void setResultOfAddPlayListTrack(const QJsonObject &p_jsonData);
    void setResultOfNewPlayListTrack(const QJsonObject &p_jsonData);
};

#endif // PROC_ADDROSETUBEINPLAYLIST_H
