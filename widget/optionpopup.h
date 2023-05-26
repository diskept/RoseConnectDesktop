#ifndef OPTIONPOPUP_H
#define OPTIONPOPUP_H
#include "common/global.h"
#include <QDialog>
#include <QJsonArray>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenu>
#include <common/filedownloader.h>
#include <common/linker.h>
#include <widget/clickablelabel.h>
#include <data/datapopup.h>

/** ******************************************************************************************************
 * ****************************************************************************************************** */
/**
 * @brief The OptionMenuActionBox class
 */
class OptionMenuActionBox : public QMenu
{
    Q_OBJECT
public:
    OptionMenuActionBox(QWidget *parent_widget = nullptr);
    QMenu *menu_subAddPlayList;
protected:
private:
    QMenu *menu_total;
    QWidget *parent_widget;
    void hideEvent(QHideEvent *) override{
        // QMenu특성때문에(?) Action에 hover 또는 trigger가 한번이라도 되면 QMenu만 사라져버리고 QMenu외 상단Content만 보인다. 그래서 부모도 hide 를 별도 처리해야한다.
        //this->parent_widget->hide();
    }
};

/** ******************************************************************************************************
 * ****************************************************************************************************** */
/**
 * @brief The MenuBox class
 */
class OptionPopup : public QDialog
{
    Q_OBJECT
public:

    /** * @brief The TypeMenu enum 옵션팝업이 띄워지는 곳(부모)의 유형을 말한다. 유형에 따라 메뉴 구성이 달라진다. */
    enum TypeMenu {
        Music_Home_RecentAlbum                      ///< 음악>홈>최근재생앨범 FrameWidget::Type::RecentAlbum
        ,Music_Home_AddedAlbum                      ///< 음악>홈>최근추가된앨범 FrameWidget::Type::AddedAlbum
        ,Music_Home_MyPlayList                      ///< 음악>홈>My플레이리스트 FrameWidget::Type::MyPlayList
        ,Music_Home_FriendPlayList                  ///< 음악>홈>친구플레이리스트 FrameWidget::Type::FriendPlayList
        ,Music_Home_Artist                          ///< 음악>홈>추천아티스트 FrameWidget::Type::Artist
        ,Music_Category_Track                       ///< 음악>분류>곡 FrameWidget::Type::MusicTrack
        ,Music_Category_Genre                       ///< 음악>분류>장르 FrameWidget::Type::MusicGenre
        ,Music_Category_Artist                      ///< 음악>분류>아티스트 FrameWidget::Type::MusicArtist
        ,Music_Category_Composer                    ///< 음악>분류>작곡가 FrameWidget::Type::MusicComposer
        ,Music_Collection_Track                     ///< 음악>마이컬렉션>트랙
        ,Music_Collection_Track_Unidentified        ///< 음악>마이컬렉션>트랙 미확인(DB 미존재)
        ,Music_Album                                ///< 음악>앨범   FrameWidget::Type::MusicAlbum
        ,Music_Folder                               ///< 음악>폴더   FrameWidget::Type::MusicFolder
        ,Music_MyCollection                         ///< 음악>마이컬렉션
        ,Music_Etc_AlbumDetailTrack                 ///< 음악>기타>앨범상세화면의 Track
        ,Music_Etc_ArtistDetailTrack                ///< 음악>기타>아티스트상세화면의 Track
        ,Music_Etc_PlayListDetailInfo               ///< 음악>기타>플레이리스트상세화면의 기본정보     FrameWidget::Type::MusicPlayListDetailInfo
        ,Music_Etc_PlayListDetailTrack_My           ///< 음악>기타>마이 플레이리스트상세화면의 Track        FrameWidget::Type::MusicPlayListDetail
        ,Music_Etc_PlayListDetailTrack_Friend       ///< 음악>기타>친구 플레이리스트상세화면의 Track        FrameWidget::Type::MusicPlayListDetail
        ,Music_Etc_RelationMusic                    ///< 음악>기타>연관 컨텐츠

        ,Video

        ,Radio

        ,RoseTube_Common                            ///< 로즈튜브>홈....및.. 기본   FrameWidget::Type::RoseTube
        ,RoseTube_Home_PlayList_My                  ///< 로즈튜브>홈>플레이리스트(마이)    FrameWidget::Type::RoseTubePlayList
        ,RoseTube_Home_PlayList_Friend              ///< 로즈튜브>홈>플레이리스트(친구)    FrameWidget::Type::RoseTubePlayList
        ,RoseTube_Etc_PlayListDetailInfo            ///< 로즈튜브>기타>플레이리스트상세화면의 기본정보  FrameWidget::Type::RoseTubePlayDetailInfo
        ,RoseTube_Etc_PlayListDetailTrack_My        ///< 로즈튜브>기타>플레이리스트상세화면의 Track(마이)    FrameWidget::Type::RoseTubePlayDetailTrack
        ,RoseTube_Etc_PlayListDetailTrack_Friend    ///< 로즈튜브>기타>플레이리스트상세화면의 Track(친구)    FrameWidget::Type::RoseTubePlayDetailTrack
        ,RoseTube_Etc_RelationVideo                 ///< 로즈튜브>연관 컨텐츠

        ,QueuePlay                                  ///< 큐 재생목록의 Track      FrameWidget::Type::QueuePlay

        ,TidalPlayList                              ///< 타이달>플레이리스트?     FrameWidget::Type::TidalPlayList
        ,TidalAlbum                                 ///< 타이달>앨범?            FrameWidget::Type::TidalAlbum

        ,Common_Etc_PlayFullScreenCurrTrack         ///< 공통>기타>재생전체화면의 현재재생중인 음원
    };

    explicit OptionPopup(QWidget *parent = nullptr);
    ~OptionPopup();

    void showMenu(int ax, int ay);
    void setDataPopup(QJsonObject p_data);
    DataPopup* getFirstDataPopup();
    QString getAlbumImg();

signals:
    void signal_clickedMovePage(const QJsonObject &p_data);
    void signal_clickedMovePage_mustMainMenu(int mainMenuCode, const QJsonObject &p_data);
    void signal_clicked_play();

public slots:
    void slot_clickedPlay();            // 바로듣기
    void slot_clickedPlayRoseTube(QJsonObject&);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_loadImage();
    void slot_clickedStar();            // 별 클릭
    void slot_gotoPage();               // 페이지를 이동하라
    void slot_clickedAddPlayList();     // 플레이리스트에 담기
    void slot_clickedDelete();          // 삭제하라

    // 재생관련 slot
    void slot_clicked_subAddLast();
    void slot_clicked_subAddEmpty();
    void slot_clicked_subAddCurrNext();
    void slot_clicked_subPlayFromCurr();
    void slot_clicked_subPlayFromCurrEmpty();
    void slot_clicked_subPlayFromCurrLast();


private:
    Linker *linker;
    // >>>>>>>>>> DATA >>>>>>>>>>>
    int mainMenuCode;       ///< 현재 메인메뉴페이지의 Code (음악or비디오 에 따라 바로듣기 api 호출 달라짐)
    TypeMenu type_optionMenu;  ///< 옵션팝업을 어느곳에서 띄우는지(부모가 누군지) 유형
    QString albumImg;       ///< 앨범 이미지 경로
    QString title;          ///< 노래 제목
    QString artist;         ///< 가수명
    QString mime;
    int cntStar = 0;        ///< 별 점수
    bool isAlbum = false;   ///< 음원이냐 앨범이냐 여부 (기본이 음원이다. 별정보 GET/SET 할때 사용한다)
    int playListNo = 0;     ///< [음악/로즈튜브] 플레이리스트 No PK
    bool flagPlayListMy = false;            ///< [음악] 플레이리스트 유형 (마이/친구)
    QList<DataPopup*> list_audioInfo;       ///< 오디오 정보 리스트 (바로듣기 api관련 된 것때문에 여러정보를 가지고있음)
    QList<DataPopup*> list_audioInfo_total; ///< 오디오 정보 리스트 (바로듣기 api관련 된 것때문에 여러정보를 가지고있음)
    int track_idx = 0;
    QJsonObject jsonData_other;             ///< 로즈튜브 플레이리스트(마이/친구)를 위한 데이터 객체
    QJsonObject jsonData_other_rosetubeSub; ///< 로즈튜브 구독정보 위한 데이터 객체
    QString playurl = "";                   ///< 로즈튜브의 별정보 GET 을 위한 값
    QString duration = "";
    QJsonArray jsonData_other_music_add_playlist; ///< 음악-플레이리스트 담기를 위한 값
    // <<<<<<<<<< DATA <<<<<<<<<<<

    // 상단 컨텐츠
    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QLabel *lb_albumImg;
    QLabel *lb_title;
    QLabel *lb_singer;
    QWidget *widget_topAlbumInfo;
    QList<ClickableLabel*> *list_lb_star;
    QWidget *widget_star;
    QVBoxLayout *vl_topContent;

    // 메뉴와 액션 관련
    QMenu *menu;
    QMenu *menuSub;
    QAction *act_play;
    QAction *act_addPlayList;
    QAction *act_metaInfo;
    QAction *act_goArtist;
    QAction *act_goAlbum;
    QAction *act_goPlayList;
    QAction *act_fileInfo;
    QAction *act_multi;
    QAction *act_edit;
    QAction *act_delete;
    QAction *act_sub;

    // 서브메뉴 액션 관련
    QAction *act_sub_addLast;
    QAction *act_sub_addEmpty;
    QAction *act_sub_addCurrNext;
    QAction *act_sub_playFromCurr;
    QAction *act_sub_playFromCurrEmpty;
    QAction *act_sub_playFromCurrLast;

    QVBoxLayout *vl_total;
    QWidget *widget_total;
    QVBoxLayout *vboxlayout;

    void setUI();
    void createActions();
    void repaintStarUI();
    void requestGetStarInfo();

    OptionPopup::TypeMenu getOptionMenuType(int op_Type);
    void setJsonData(QJsonObject p_jsonObject);
    QJsonObject getJsonDataOptionPopup();                ///< 옵션팝업에서 사용하는 데이터 정보 반환
    void setUIDataFromMemberData();
    void clearInitMenuAction();
    void setPopupMenuFromMode();

};

#endif // OPTIONPOPUP_H
