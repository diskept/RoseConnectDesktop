#ifndef ARTISTINFO_H
#define ARTISTINFO_H

#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>

#include <data/datapopup.h>

#include <common/filedownloader.h>
#include <common/linker.h>

#include <widget/flowlayout.h>
#include "widget/optionpopup.h"
#include "proc_addtackinplaylist.h"



/**
 * @brief 아티스트 상세 화면 @n
 *      아티스트의 곡, 앨범 모음
 */
class ArtistInfo : public QWidget
{
    Q_OBJECT
public:
    explicit ArtistInfo(QWidget *parent = nullptr);
    ~ArtistInfo();

    void setDataArtistInfo(QJsonObject p_data);

signals:
    void signal_changedMenuName(const QString &p_menuName);
    void signal_clickedViewAll(QString p_viewAllMode);


public slots:
    void slot_loadImage();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_playAll();
    void slot_playShuffle();
    void slot_playlistClickedTrack();
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);
    void slot_clickedTrackViewAll();
    void slot_clickedAlbumViewAll();

private :
    // >>>>>>>>>> DATA >>>>>>>>>>>
    OptionPopup::TypeMenu type_optionMenu;  ///< 옵션팝업을 어느곳에서 띄우는지(부모가 누군지) 유형
    QString albumImg;                   ///< 앨범 이미지 경로
    QString artist;                     ///< 가수명
    QString infoText;                   ///< 가수 설명
    QList<DataPopup*> list_audioInfo;   ///< 오디오 정보 리스트 (바로듣기 api관련 된 것때문에 여러정보를 가지고있음)
    // <<<<<<<<<< DATA <<<<<<<<<<<

    Linker *linker;

    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QLabel *lb_tracks;
    QLabel *lb_albums;
    QLabel *lb_albumImg;
    QLabel *lb_artist;
    QLabel *lb_infoText;
    QVBoxLayout *vl_tracks;
    //FlowLayout *fl_albums;
    QHBoxLayout *boxAlbums;

    Proc_AddTackInPlayList *proc_add;

    void setUIControl();
    void setDataTrackFromDB();
    void setDataAlbumFromDB();
    QJsonArray getJsonArr_TrackData(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);

};

#endif // ARTISTINFO_H
