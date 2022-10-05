#ifndef PROC_ADDTACKINPLAYLIST_H
#define PROC_ADDTACKINPLAYLIST_H

#include <QWidget>
#include <QFile>
#include <common/filedownloader.h>
#include "login/dialog_playlist.h"
#include "login/dialog_newplaylist.h"
#include "common/linker.h"

/**
 * @brief 음악 플레이리스트 담기 Proc @n
 *      @sa Dialog::Dialog_Playlist 를 이용해 플레이리스트 선택 후 담기 지원 @n
 *      신규 플레이리스트 담기의 경우 썸네일 업로드
 */
class Proc_AddTackInPlayList : public QWidget
{
    Q_OBJECT
public:
    explicit Proc_AddTackInPlayList(QWidget *parent = nullptr);
    ~Proc_AddTackInPlayList();

    void setTrackJsonArray(QJsonArray p_jsonArray);
    void showDlgOfPlayList();
    void showDlgOfNewPlayList();
    void requestAddPlayListTrack(const int &p_playlistId);
    void requestAddPlayListTrack(const int &p_playlistId, const QJsonArray &p_jsonTracks);
    void requestNewPlaylistTrack(const QString &p_name);
    void requestNewPlaylistTrack(const QPixmap &pixmap, const QString &p_firstPlayUrl, const QString &p_name, const QJsonArray &p_jsonTracks);
    void requestNewPlaylistTrack(const QString &p_name, const QJsonArray &p_jsonTracks);

signals :
    void signal_playlistClicked(const int &p_playlistno);
    void signal_newPlaylistNameEdited(QString p_name);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_networkManagerFinished();
    void slot_replyFinished();
    void slot_loadImage();

private :



    Linker *linker;
    QFile *fileUpload;
    FileDownloader *fileDownloader;

    Dialog::Dialog_Playlist *dlg_playList;
    QPixmap pixmap_albumImg;
    QString playUrl;

    QJsonArray jsonArrTracks;

    void setInit();
    void setUIControl();
    void setResultOfAddPlayListTrack(const QJsonObject &p_jsonData);
    void setResultOfNewPlayListTrack(const QJsonObject &p_jsonData);
    void saveAlbumImgToFile(const QString &p_playUrl);
};

#endif // PROC_ADDTACKINPLAYLIST_H
