#ifndef PLAYFULLSCREENRELATION_H
#define PLAYFULLSCREENRELATION_H

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <common/filedownloader.h>
#include <common/linker.h>

#include <data/dataStructs.h>
#include <data/datapopup.h>

#include <delegate/musicrelationdelegate.h>
#include "delegate/rosetuberelationdelegate.h"
#include "music/proc_addtackinplaylist.h"
#include "rosetube/proc_addrosetubeinplaylist.h"
//#include "YoutubeWebGet_global.h"
//#include "YoutubePlayerPreview.h"
#include "clickablelabel.h"

// for Tidal & Bugs
#include "widget/OptMorePopup.h"

#include "textlonganimation.h"
#include <QDialog>


/**
 * @brief The OptionPopupVideoDisplay class 전체재생화면에서 사용하는 비디오의 디스플레이 설정 옵션팝업 클래스
 * @note 전체재생화면(PlayFullScreenRelation) 내부에서 사용하는 클랙스이다.
 */
class OptionPopupVideoDisplay : public QDialog
{
    Q_OBJECT
public:
    explicit OptionPopupVideoDisplay(QWidget *parent = nullptr);
    ~OptionPopupVideoDisplay();

    void setCurrDisplayIndex(int p_index);

private slots:
    void slot_setSettingValue();

private:
    int curDisplayIndex = 0;    ///< 현재 세팅된 디스플레이의 인덱스

    // 메뉴와 액션 관련
    QMenu *menu;
    QAction *act_bestfit;       ///< 인덱스 : 0
    QAction *act_16_9;          ///< 인덱스 : 1
    QAction *act_4_3;           ///< 인덱스 : 2
    QAction *act_center;        ///< 인덱스 : 3
};


/**
 * @brief The PlayFullScreenRelation class 재생화면 (with연관컨텐츠)
 * @note PlayFullScreenRelation 클래스 : 곡 리스트 클래스 <RelationAudioInfo 여러개 가지고 있는 클래스>
 */
class PlayFullScreenRelation : public QWidget
{
    Q_OBJECT
public:
    explicit PlayFullScreenRelation(QWidget *parent = nullptr);
    ~PlayFullScreenRelation();

signals:
    // AbstractMainContent 클래스의 - signal_mustChangeMainMenuAndMovePage 시그널 발생 연결을 위한 시그널임. (현 구조에서 할 수 있는 방안)
    void signal_mustChangeMainMenuAndMovePage(int p_mainMenuCode, const QJsonObject &p_data);

public slots:
    void setDataPlayFullScreen(const QJsonObject &p_jsonData);

private slots:
    void slot_loadImage();
    void slot_clickedStar();            // 별 클릭
    void slot_clickedBtn_addCollection();
    void slot_clickedBtn_sub();
    void slot_clickedBtn_fav();
    void slot_clickedBtn_more();
    void slot_clickedVideoProp();
    void slot_clickedHDMI();

    // rosetue relations video thumbnails
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData);
    void slot_thumbnailDownloaded();
    void slot_delegateClicked_roseTube(const int &p_index, const int &p_btnType);
    void slot_delegateClicked_music(const int &p_index, const int &p_btnType);


    // for YoutubePlayerPreview
    //void slot_ytbPlayerPreview_endedGetDescriptionInfo(QString descHtml, QString descText);
    //void slot_ytbPlayerPreview_endedGetMoreRelatedVideoList(int index_start, int count);
    //void slot_ytbPlayerPreview_endedGetAllData(int count);
    //void slot_ytbPlayerPreview_occurError(YoutubePlayerPreview::ErrorType errType);

    void slot_playlistClicked_Rosetube(const int &p_playlistno);
    void slot_newPlaylistNameEdited_Rosetube(QString);
    void slot_redirectUrl(const QString url);   //j220903 twitter


private:
    Linker *linker;

    QString playType = "";  ///< 현재 재생음원의 유형 (음악/비디오/라디오/로즈튜브/타이달..등등)
    QString albumName;      ///< 앨범명
    QString titleName;      ///< 노래제목
    QString artistName;     ///<  가수이름
    QString path = "";      ///< 음원 경로
    int curPosition = 0;    ///< 현재 재생 시점 value
    int duration = 0;       ///< 재생 길이
    bool isFavorite = false;///< 하트 유무
    bool isHdmiOn = false;
    bool isPlaying = false;
    bool isServer = false;  ///< 네트워크 파일인지 유무 인듯.. (마이/친구/폴더 파일인경우에만 true)
    bool isShareFile = false;   ///< 음악에서만 사용하는... (공유한 파일인지의 유무... 친구 음원인경우에만 true 이다.. 네트워크폴더 파은 false 이다)
    int repeatMode = 0;
    int shuffleMode = 0;
    QString audio_id = "";  ///< 오디오 PK
    int album_id = 0;
    int cntStar = 0;        ///< 별 정보
    QString artist_id = "";
    QString thumbnail;      ///< 앨범 이미지 경로
    QString rosetueChannelId;           ///< 로즈튜브 채널 아이디
    QString rosetubeVideoId;            ///< 로즈튜브 비디오 아이디
    QString rosetubeRelationVideoId;    ///< 로즈튜브 연관 컨텐츠 비디오 아이디
    bool isSelect_radio = false;        ///< 라디오에서 사용하는 변수
    QString radio_url = "";             ///< 라디오에서 사용하는 URL
    bool isSubscribe = false;           ///< 구독 유무 (로즈튜브,팟캐스트에서 사용)

    OptionPopupVideoDisplay *optionPopupVideoDisplay;     ///< 비디오 디스플레이 옵션팝업
    QPushButton *btn_hdmi;                  ///< 영상 hdmi on/off 버튼
    QPushButton *btn_icon_video_prop;       ///< 비디오 디스플레이 사이즈 버튼
    QPushButton *btn_play_list_add_icon;    ///< 플레이리스트 담기 버튼
    QPushButton *btn_play_sub;          ///< 구독 버튼
    QPushButton *btn_list_fav_icon;     ///< 하트 버튼
    QPushButton *btn_icon_menu;         ///< 더보기 버튼

    QLabel *lb_fileInfo;
    //QLabel *lb_title;
    TextLongAnimation *lb_title;
    QLabel *lb_artistName;
    QLabel *lb_albumThumb;
    QLabel *lb_albumThumbText;
    QList<ClickableLabel*> *list_lb_star;
    QPixmap *pixmap_albumImg=nullptr;
    FileDownloader *filedownloader;
    QWidget *widget_starOnly;
    QWidget *widget_relationList;

    QWidget *widget_playInfo;

    //QWidget *widget_relationRosetube;
    QListWidget *listWidget_relation;   ///< 연관컨텐츠 ListWidget
    RoseTubeRelationDelegate *delegate;
    MusicRelationDelegate *delegateMusic;

    QList<DataPopup*> *list_dataPopup;

    QString ytbLocalKey;
    //YoutubePlayerPreview *ytbPlayerPreview;                 ///< Youtube Crawling class
    Proc_AddRosetubeInPlayList *proc_addPlaylist_RoseTube;

    void setInit();
    void setUIControl();
    void setDataJson(const QJsonObject &p_jsonData);
    void setUIfromData();
    void setBackgroundAlbumImg();
    void setDataRelationFromDB_music();
    void setDataRelationFromDB_youtube();
    void setShowHideBtnVideoSettingUI(QJsonObject p_jsonObject);
    void repaintStarUI();
    void repaintHearUI();
    void repaintSubscribeUI();
    void requestVideoSetting();
    void requestGetStarInfo();
    void requestGetHartInfo();
    void requestGetSubInfo();

    QJsonObject getJsonObject_TrackDataCurr();
    QJsonObject getJsonObject_TrackData(const int &p_index);
    void appendMusicTrack(const QJsonObject &p_jsonData);
    void requestGetMusicTrackFavorites(const int &p_index);
    void requestSetMusicTrackFavorites(const int &p_index);
    void requestPlayMusic(const int &p_index);
    void setResultOfGetMusicFavorite(const int &p_index, const QJsonObject &p_jsonData);
    void setResultOfSetMusicFavorite(const int &p_index, const QJsonObject &p_jsonData);

    // rosetube relation
    QString getVideoIdFromThumbnail();
    QString getVideoIdFromUrl(QString p_playUrl);
    void requestRelationVideo_youtube(const QString &p_videoId);
    void setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor);
    void changeTrackFavoritesUI(const int &p_index);
    void requestGetRosetubeTrackFavorite(const int &p_index, const QString &p_playUrl);
    void requestSetRosetubeTrackFavorite(const int &p_index, const QString &p_playUrl);
    void requestPlayRosetube(QJsonArray jsonPlayList);
    void appendRosetubeTrack(const QJsonObject &p_jsonData);
    void appendRosetubeTrack_NEW(const QJsonObject &p_jsonData);
    void setResultOfGetRosetubeTrackFavorite(const int &p_index, const QJsonObject &p_jsonData);
    void setResultOfSetRosetubeTrackFavorites(const int &p_index, const QJsonObject &p_jsonData);
    void setResultOfGetRelationVideoList(const QJsonObject &p_jsonData);


    // override
    void resizeEvent(QResizeEvent *event) override;
    //void showEvent(QShowEvent *event) override;
    //void initPainter(QPainter *painter) const override;
    void paintEvent(QPaintEvent *event) override;




//------------------------------------------------------------------------------------
// for Tidal, Bugs
//------------------------------------------------------------------------------------
private:
    QJsonObject jsonObj_subAppCurrentData;                      // Tidal, Bugs는 이 정보가 필요함. 멤버 변수로 저장함

    void showDialog_toAddMyCollection();                        // Tidal 플레이리스트 담기 다이얼로그 띄우기

    void makeObj_optMorePopup(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int section);

private slots:
    void slot_completeReq_flagFav_onTrack(const int track_id, const bool flagFavOn);        // Tidal Track > Fav 기본값 응답에 대한 처리
    void slot_completeReq_flagFav_onVideo(const int video_id, const bool flagFavOn);        // Tidal Video > Fav 기본값 응답에 대한 처리

    void slot_add_choosePlaylist_forTrack(QString playlist_uuid_target, int track_id);      // Tidal > 플레이리스트 담기 Dlg 선택에 대한 처리 (from Dialog)
    void slot_done_addPlaylist();

    void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section);

};

#endif // PLAYFULLSCREENRELATION_H
