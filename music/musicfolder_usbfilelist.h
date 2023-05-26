#ifndef MUSICFOLDER_USBFILELIST_H
#define MUSICFOLDER_USBFILELIST_H

#include "music/proc_addtackinplaylist.h"

#include "common/linker.h"

#include "data/datanetworkfolder.h"

#include "delegate/usbfiledelegate.h"

#include "widget/folderwidgetgroup.h"
#include "widget/OptMorePopup.h"
#include "widget/dialogconfirm.h"

#include <QJsonArray>
#include <QListWidget>
#include <QWidget>

/**
 * @brief 음악-폴더-USB 저장장치-파일 목록
 *
 * @note http://ip:port/external_usb API 사용하여 파일 목록 구성
 */
class MusicFolder_USBFileList : public QWidget
{
    Q_OBJECT
public:
    explicit MusicFolder_USBFileList(QWidget *parent, bool flagIsMusic = true);
    void setData(const QJsonObject &p_jsonData);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_changedRootPath(const QString &p_dirName , const QString &p_dirPath);
    void slot_delegateClicked(const int &p_index, const int &p_btnType);
    void slot_thumbnailDownloaded();
    void slot_clickedPlayAll();
    void slot_clickedPlayShuffle();
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);
    void slot_responseHttp_stop(const int &p_id, const QJsonObject &p_jsonObject);//c220714
    void slot_responseHttp_play(const int &p_id, const QJsonObject &p_jsonObject);//c220714

private:
    void setInit();
    void setInitMediaSrcData();
    void setUIControl();
    void setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor);
    void changeTrackFavoritesUI(const int &p_index);
    void appendFileUI(const QJsonObject &p_jsonObject);
    void requestGetTrackFavorites(const int &p_index, const QString &p_fielPath);
    void requestSetTrackFavorites(const int &p_index);
    void requestAddTrackInPlaylist(const int &p_index);
    void setResultOfGetFavorite(const int &p_idx, const QJsonObject &p_jsonData);
    void setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData);
    QJsonObject getFileInfoFromDB(const QString &p_path);
    QJsonObject getPlayMusicInfoObject(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);
    QJsonArray getPlayMusicInfoArray(const int &p_index);
    void dialog_comfirmCD_Stop();//c220714
    void playMusic(QJsonArray jsonArrMusic, int type, bool flagPlayShuffle=false);
    void showOptionPopup(int);
    void showDlgOfAddFaroviteFolder(const QString &p_name, const QString &p_path);

    void requestFileList();
    void setResultOfFileList(const QJsonObject &p_jsonObject);

    QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
    QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
    QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

private:
    Linker *linker;

    bool flagIsMusic;

    QJsonArray jsonArrMediaSrc;

    Folder_TopDirPath *topDirPath;

    USBFileDelegate *delegate;
    QListWidget *listWidget;

    DataNetworkFolder *data;
    QList<DataNetworkFolder*> *listFileData;

    QPixmap pixmapThumb;
    Proc_AddTackInPlayList *procAdd_playlist;

    QPushButton *btn_play;
    QPushButton *btn_shuffle;

    int select_index = 0;
};

#endif // MUSICFOLDER_USBFILELIST_H
