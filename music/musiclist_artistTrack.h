#ifndef MUSICLIST_ARTISTTRACK_H
#define MUSICLIST_ARTISTTRACK_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include <data/datapopup.h>

#include <common/filedownloader.h>
#include <common/linker.h>

#include <widget/flowlayout.h>
#include "widget/optionpopup.h"
#include "proc_addtackinplaylist.h"

class MusicList_ArtistTrack : public QWidget{
    Q_OBJECT

public:
    explicit MusicList_ArtistTrack(QWidget *parent = nullptr);
    ~MusicList_ArtistTrack();

    void requestInitData(QJsonObject jsonObj);

public slots:
    void slot_loadImage();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedPlayAll();
    void slot_clickedPlayShuffle();
    void slot_playlistClickedTrack();
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);


private:
    void setInit();
    void setUIControl();

    void requestTracks();

    QJsonArray getJsonArr_TrackData(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);

    void wheelEvent(QWheelEvent *event) override;


private:
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
    QLabel *lb_albumImg;

    QVBoxLayout *vl_tracks;

    Proc_AddTackInPlayList *proc_add;

    QString artist_name;
    bool flagReqMore = false;
    int totalCount = 0;
    int offset = 0;

};

#endif // MUSICLIST_ARTISTTRACK_H
