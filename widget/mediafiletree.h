#ifndef MEDIAFILETREE_H
#define MEDIAFILETREE_H

#include <QWidget>
#include <QUuid>
#include <QFileSystemModel>
#include <delegate/filedelegate.h>
#include <widget/customlistwidget.h>
#include <common/linker.h>
#include "data/datapopup.h"
#include "music/proc_addtackinplaylist.h"
#include "model/networkfilesystemmodel.h"
#include "widget/dialogconfirm.h"//cheon210812-iso

/**
 * @brief 네트워크 미디어 파일 목록 위젯 @n
 *      @sa MediaFileTree::setData(QString,QString) 네트워크 path 세팅 @n
 *      해당 경로의 파일 목록으로 구성@n
 *
 * @note
 *      프리징 오류 수정 @n
 *      QFileSystemModel 관련 버그 :: 특정 폴더(대용량)에 2번째 이동시 LOCK 발생 @n
 *      :: RootPath 변경시마다 부모객체에서 MediaFileTree new 생성
 *      :: 추후 QFileSystemModel 사용하지 않고 QAbstrackItemModel 구현 고려
 *      QFileSystemModel + QListWidget 사용 문제 :: 파일이 많은경우 LOCK 발생 @n
 *      :: QFileSystemModel 상속받은 @sa NetworkFileSystemModel + QListView 사용으로 변경
 */
class MediaFileTree : public QWidget
{
    Q_OBJECT

    struct FileLoadStruct{
        QUuid uuid;
        QString filePath = "";
        QString fileName = "";
        int index = -1;
        bool flagIsDir = false;
    };

public:
    explicit MediaFileTree(QWidget *parent = nullptr, bool flagIsMusic=true);
    ~MediaFileTree();

    void setData(const QString &p_rootName, const QString &p_rootPath);
    void slot_changedRootPath(const QString &p_dirName , const QString &p_dirPath);

signals:
    void signal_clickedDirName(QString, QString);
    void signal_isoplay(bool a);//cheon210617-http

private slots:

    void slot_directoryLoaded(QString);
    void slot_reqThumbnail(const int &p_index, const QString &p_filePath, const QString &p_fileName);
    void slot_reqFavorites(const int &p_index);
    void slot_loadedMediaFile(const QJsonObject &p_json, const QPixmap &p_pixmap);
    void slot_delegateClicked(const int &p_index, const int &p_btnType);
    //void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);//cheon210812-iso
    void slot_responseHttp_play(const int &p_id, const QJsonObject &p_jsonObject);//cheon210812-iso
    void slot_responseHttp_stop(const int &p_id, const QJsonObject &p_jsonObject);//cheon210812-iso
    void slot_iso_play();//c220711
    void slot_clickedPlayAll();
    //void slot_clickedPlayAllafterCheck(const bool &a);//cheon210812-iso
    void slot_clickedPlayShuffle();
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);

private :

    const int HTTP_NETWORK_PLAY = 99;
    const int HTTP_NETWORK_FAVORITE_GET = 98;
    const int HTTP_NETWORK_FAVORITE_ADD = 58;
    const int HTTP_NETWORK_FAVORITE_DELETE = 57;

    Linker *linker;
    DialogConfirm *dlgConfirmCD;//cheon210812-iso
    QJsonArray jsonArrMusic_curr;//cheon210812-iso
    bool flagIsMusic;

    QUuid uuid;
    QString currSMBPath;
    QString currRootPath;
    NetworkFileSystemModel *model;
    FileDelegate *delegate;
    QListView *listView;

    int loadMediaLastIndex;
    QList<FileLoadStruct> *list_fileLoadStruct;

    QPixmap pixmapThumb;
    Proc_AddTackInPlayList *procAdd_playlist;

    void dialog_comfirmCD();//cheon210812-iso
    void dialog_comfirmCD_Stop();//c220711
    void setInit();
    void setUIControl();
    void setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor);
    void changeTrackFavoritesUI(const int &p_index);
    void requestGetTrackFavorites(const int &p_index);
    void requestSetTrackFavorites(const int &p_index);
    void setResultOfNetworkPlay(const QJsonObject &p_jsonData);
    //void setResultOfNetworkPlay_notiso(const QJsonObject &p_jsonData);//cheon210812-iso

    void setResultOfGetFavorite(const int &p_idx, const QJsonObject &p_jsonData);
    void setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData);
    QJsonObject getPlayMusicInfoObject(const int &p_index, DataPopup *p_dataPopup);
    QJsonObject getJsonObject_TrackData(const int &p_index);
    QJsonArray getPlayMusicInfoArray(const int &p_index);
    void playMusic(QJsonArray jsonArrMusic, bool flagPlayShuffle=false);
    void playMusic_afterStopCheck(QJsonArray jsonArrMusic, bool flagPlayShuffle=false);//cheon210812-iso
    void PlayAllafterStopCheck(const bool &flag);//cheon210812-iso
    void showDlgOfAddNetworkFolder(const QString &p_dirName);
    void showDlgOfAddFaroviteFolder(const QString &p_dirName);
    void setConvertedSMBPath(QString p_smbPath);
    void requestAddTrackInPlaylist(QString p_fileName);
    QString getSMBID();
    QString getSMBPW();
    QString convertSMBPathToFilePath(const QString &p_smbPath);
    QString createSMBPath(bool flagAddIdPw=false);
    void showOptionPopup(int p_index);
    DataPopup* getTrackData(const int &p_index);
    QJsonObject convertDataPopupToJsonObject(DataPopup*);
    void loadMediaMimeData(const int &p_index);

    QPushButton *btn_play;
    QPushButton *btn_shuffle;
};

#endif // MEDIAFILETREE_H
