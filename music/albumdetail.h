#ifndef ALBUMDETAIL_H
#define ALBUMDETAIL_H

#include "music/proc_addtackinplaylist.h"
#include "music/proc_uploadthumbnail.h"

#include "data/datapopup.h"

#include "common/filedownloader.h"
#include "common/linker.h"

#include "home/abstractpage.h"
#include "home/topmenubar.h"

#include "widget/clickablelabel.h"
#include "widget/flowlayout.h"
#include "widget/optionpopup.h"
#include "widget/OptMorePopup.h"//c211213

#include <QDialog>
#include <QFile>
#include <QLabel>
#include <QStackedWidget>
#include <QWidget>


/**
 * @brief The AlbumDetail class 앨범상세 화면 @n
 *      앨범 관리(플레이리스트담기, 별점, 하트, 앨범 정보) 및 트랙 관리(재생, 플레이리스트담기, 하트, 별점, 메타정보) 지원
 * @note @sa AbstractPage 상속
 */
class AlbumDetail : public AbstractPage
{
    Q_OBJECT
public:
    explicit AlbumDetail(QWidget *parent = nullptr);
    ~AlbumDetail();

    void setDataABSPage(QJsonObject p_data) override;

signals:
    void signal_clickedMovePage(const QJsonObject &p_data);
    void signal_changedMenuName(const QString &p_menuName);

public slots:
    void slot_changedSubTabUI(const QJsonObject &p_data) override;
    void slot_loadImage();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_requestAlbumFavorite();
    void slot_clickedStar();
    void slot_openDlgInfo();
    void slot_playAll();
    void slot_playShuffle();
    void slot_addPlayListTrack();
    void slot_playlistClicked(const int &p_playlistno);
    void slot_playlistClickedTrack();
    void slot_newPlaylistNameEdited(QString);

private:
    // >>>>>>>>>> DATA >>>>>>>>>>>
    OptionPopup::TypeMenu type_optionMenu;  ///< 옵션팝업을 어느곳에서 띄우는지(부모가 누군지) 유형
    QString albumImg;                   ///< 앨범 이미지 경로
    QString albumName;                  ///< 앨범명
    int album_id = 0;
    QString albumKey = "";
    QString artist;                     ///< 가수명
    int cntStar = 0;                    ///< 별 점수
    int cntStarTmp = 0;
    QList<DataPopup*> list_audioInfo;   ///< 오디오 정보 리스트 (바로듣기 api관련 된 것때문에 여러정보를 가지고있음)
    bool isFavorite = false;            ///< 좋아요 여부 (앨범)
    // <<<<<<<<<< DATA <<<<<<<<<<<

    Linker *linker;

    QPixmap pixmap_heart_on;
    QPixmap pixmap_heart_off;
    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QLabel *lb_albumImg;
    QLabel *lb_title;
    QLabel *lb_artist;
    QList<ClickableLabel*> *list_lb_star;
    QPushButton *btn_heart;

    QFile *fileUpload;

    TopMenuBar *menubar;
    QStackedWidget *stackedwidget;
    QVBoxLayout *vl_tracks;
    QVBoxLayout *vl_cradit;
    FlowLayout *fl_albumRecommend;

    Proc_AddTackInPlayList *proc_add;
    int curr_clickMode = 16;//c211213

    virtual QJsonObject getDataJson() override;                                      //
    void setInit();
    void setUIControl();
    void setAlbumArtFromDB();
    void setDataTrackFromDB();
    void setDataCraditFromDB();
    void setDataRecommendFromDB();
    void setAlbumkeyFromDB();
    void setStarPointUI();
    void setFavoriteUI();
    void changedOnlyTabUI_notSendSignal(QString p_step);
    QJsonArray getJsonArr_TrackData(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);
    QStringList getPlayUrlList(const int &p_index);
    int get_rose_playType(OptMorePopup::ClickMode clickMode);//c211213

    void setTitleAlbum();
    void requestIsStar();
    void requestIsfavorite();
    void requestAddPlayListTrack(const int &p_playlistId, const QJsonArray &p_jsonTracks);
    void requestNewPlaylistTrack(const QString &p_name, const QJsonArray &p_jsonTracks);
    void setResultOfAddPlayListTrack(const QJsonObject &p_jsonData);
    void setResultOfNewPlayListTrack(const QJsonObject &p_jsonData);
    void setResultOfGetStar(const QJsonObject &p_jsonData);
    void setResultOfSetStar(const QJsonObject &p_jsonData);
    void setResultOfSetFavorites(const QJsonObject &p_jsonData);
    void saveAlbumImgToFile(const QString &p_playUrl);
};

#endif // ALBUMDETAIL_H
