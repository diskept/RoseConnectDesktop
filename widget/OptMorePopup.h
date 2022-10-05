#ifndef OPTMOREPOPUP_H
#define OPTMOREPOPUP_H


#include <QWidget>
#include <QDialog>
#include <QMenu>
#include <QVBoxLayout>
#include <QJsonObject>

#include <common/filedownloader.h>

#include "clickablelabel.h"
#include "StarPointWidget.h"        // 별점 위젯

/**
 * @brief Album, Track, Artist, Playlist, Video 등의 팝업메뉴 Widget
 * @details Tidal, Qobuz, Bugs 에서 사용중
 */
class OptMorePopup : public QDialog
{
    Q_OBJECT
public:

    /**
     * @brief OptiMorePopup 의 상단 헤더에 대한 정보
     */
    struct HeaderData{
        QString main_title;         ///< 옵션팝업 메인 타이틀 (흰색 큰 글씨). empty 이면 drawing 하지 않는다.
        QString sub_title;          ///< 옵션팝업 서브 타이틀 (main_title 아래의 작은 글씨 부분). empty 이면 drawing 하지 않음
        QString imageUrl;           ///< 옵션팝업 헤더에서 이미지 경로        
        QString data_pk;            ///< 각 데이터에 대한 PK값. Album의 id, Playlist의 Key값 등등.  (Star 별점처리를 위해 사용함)
        QString clientKey;          ///< Share link type별 Album의 id, Playlist의 Key값     //c220903_2
        QString type;               ///< Share link type                                    //c220903_2

        bool flagProcStar;          ///< Star(별) 포인트에 대한 처리 여부. true 인 경우에는 내부적으로 API요청하여 Get, Set 처리함
        bool flagFavOn;             ///< 좋아요 사용하는 경우에 대하여, 초기값 설정을 위함
        bool cache_lock;            ///< Cache content lock/unlock
        bool isRose;                ///< share link isRose ?        //j220905
        bool isShare;               ///< share enalbe               //j220906
        bool isMix;                 ///< share enalbe Tidal Mix     //j220906
    };


    /**
     * @brief OptMorePopup 이외 다른 곳에서도 사용하는 Enum 데이터.
     */
    enum PopupMenuMode {
        None = 0

        , Music_Album = 21
        , Music_Playlist
        , Music_Track

        , Music_Track_inFullScreen

        , Video_Alubm = 31
        , Video_Playlist
        , Video_Track

        , Video_Track_inFullScreen

        , Rosetube_Video = 41
        , Rosetube_Playlist
        , Rosetube_Track
        , Rosetube_Caching

        , Rosetube_Track_inFullScreen

        , Tidal_Album = 71
        , Tidal_Track
        , Tidal_Playlist
        , Tidal_Playlist_Created            ///< 사용자가 생성한 playlist
        , Tidal_Video
        , Tidal_Artist

        , Tidal_Track_inFullScreen

        , Bugs_Track = 81
        , Bugs_Track_withVideo              ///< 영상보기 있는 Track
        , Bugs_Album
        , Bugs_PD_Album
        , Bugs_MyAlbum
        , Bugs_Video
        , Bugs_Artist

        , Bugs_Track_inFullScreen
        , Bugs_Track_inFullScreen_withVideo
        , Bugs_Video_inFullScreen

        , Qobuz_Album = 91
        , Qobuz_Track
        , Qobuz_Playlist
        , Qobuz_Playlist_Created            ///< 사용자가 생성한 playlist
        , Qobuz_Video
        , Qobuz_Artist

        , Qobuz_Track_inFullScreen

        , Rosehome_Album = 101
        , Rosehome_Playlist
        , Rosehome_Playlist_Created            ///< 사용자가 생성한 playlist
        , Rosehome_Track
        , Rosehome_Video
        , Rosehome_Artist

        , Rosehome_Track_inFullScreen

        , Apple_Album = 111
        , Apple_Playlist
        , Apple_Playlist_Created            ///< 사용자가 생성한 playlist
        , Apple_Track
        , Apple_Video
        , Apple_Artist

        , Apple_Track_inFullScreen
    };
    Q_ENUM(PopupMenuMode);


    /**
     * @brief signal_clicked 시그널 발생 시, 어떤 클릭인지 알 수 있음
     */
    enum ClickMode {
        Play_RightNow = 1                       ///< 바로 듣기
        , Play_RightNow_withReorder             ///< 바로 듣기 (index reordering. 내부적으로 필요함) - OptMorePopup에 뷰로 보이는 녀석은 아님.
        , Add_MyCollection                      ///< 플레이리스트에 담기
        , MetaInfo                              ///< 메타정보
        , Go_Artist                             ///< Go to Artist
        , Go_Album                              ///< Go to Album
        , Go_Playlist                           ///< Go to Playlist
        , Go_Ref_Videos                         ///< 영상보기 (Bugs)
        , FileInfo                              ///< 파일 정보
        , Multi_Select                          ///< 여러항목 선택
        , Edit                                  ///< 편집
        , Delete                                ///< 삭제
        , SubscribInfo                          ///< 구독정보
        , Caching_ahead                         ///< YOUTUBE Cache

        , Add_Favorite                          ///< 즐겨찾기 추가 (타이달) - 하트 On
        , Delete_Favorite                       ///< 즐겨찾기 삭제 (타이달) - 하트 Off

        , Cache_Lock                            ///< Cache Lock
        , Cache_Unlock                          ///< Cache Unlock

        , SubMenu_QueueAdd_Last                 ///< 서브 : 큐 끝에 추가
        , SubMenu_QueueAdd_Empty                ///< 서브 : 큐 비우고 추가
        , SubMenu_QueueAdd_RightNow             ///< 서브 : 바로듣기 추가
        , SubMenu_QueueAdd_CurrNext             ///< 서브 : 현재 재생곡 다음 추가
        , SubMenu_Play_FromHere                 ///< 서브 : 여기부터 재생
        , SubMenu_Play_FromHere_procEmpty       ///< 서브 : 큐 비우고 여기부터 재생
        , SubMenu_QueueAdd_FromHere_Last        ///< 서브 : 여기부터 큐 끝에 추가

        , SubMenu_QueueAdd_Last_OnlyOne         ///< 서브 : 한 곡 큐 끝에 추가
        , SubMenu_QueueAdd_Empty_OnlyOne        ///< 서브 : 한 곡 큐 비우고 추가
        , SubMenu_Play_RightNow_OnlyOne         ///< 서브 : 한 곡 바로 듣기 추가
        , SubMenu_QueueAdd_CurrNext_OnlyOne     ///< 서브 : 한 곡 현재 재생곡 다음 추가

        , Share                         ///< YOUTUBE share//c220818
    };
    Q_ENUM(ClickMode);

    explicit OptMorePopup(QWidget *parent = nullptr);
    ~OptMorePopup();
    void setMenuMode(const PopupMenuMode curr_menuMode, const HeaderData &data_header, const int index=0, const int section=0, const bool p_flagForceHide_favorite=false);
    void showPopup();

signals:
    void signal_menuClicked(const OptMorePopup::ClickMode, const int index, const int section);

private slots:
    void slot_loadImage();
    void slot_clicked_starPoint(const int starRate);
    void slot_procStarData_complete_getStarRate(const int starRate);
    void slot_procStarData_complete_setStarRate(const int starRate);

    void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& jsonObj);
    void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& jsonObj);

private:

    // function about UI
    void setUIControl_basic();
    QPoint getHintPos();
    void setThumbnailImage(const QString &imageUrl);

    // function about starRate (별점)
    void resetUI_showStarRate(bool flagShow);
    void requestProcStarData_get();
    void requestProcStarData_set(const int starRate);


    // function about action menu
    void setUIControl_header();
    void createActionMenu();
    void add_mainAction(const OptMorePopup::ClickMode);
    void add_subAction(const OptMorePopup::ClickMode);
    QString getMenuString_byMenuMode(const OptMorePopup::ClickMode);


    // function about only Tidal
    void check_tidalFavorites();

    void check_qobuzFavorites();


    // data (to emit signal with parameters)
    OptMorePopup::PopupMenuMode curr_menuMode = OptMorePopup::PopupMenuMode::None;
    int index = 0;
    int section = 0;
    int cnt_subMenu = 0;        ///< 서브메뉴 갯수
    OptMorePopup::HeaderData data_header;

    bool flagForceHide_favorite = false;                ///< 강제로 즐겨찾기 버튼을 가릴지의 여부임.

    // Header UI
    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QLabel *lb_albumImg;
    QLabel *lb_title;
    QLabel *lb_singer;
//    QList<ClickableLabel*> *list_lb_star;
    StarPointWidget *starPointWidget;
    QVBoxLayout *vl_topContent;

    // Body for menu
    QMenu *menu;
    QMenu *menuSub;

    // total layout
    QVBoxLayout *vl_total;
    QWidget *widget_total;
    QVBoxLayout *vboxlayout;


    // Tidal 에서 Show / Hide 해야하는 action 정보
    QAction *act_addFavorite;        ///< 츨겨찾기 추가
    QAction *act_deleteFavorite;     ///< 즐겨찾기 삭제

};

#endif // OPTMOREPOPUP_H
