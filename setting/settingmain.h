#ifndef SETTINGMAIN_H
#define SETTINGMAIN_H

#include "settingappinfo.h"
#include "settingappmenu.h"
#include "settingcountry.h"
#include "settingdisplay.h"
#include "settingfriendpage.h"
#include "settinginputoutput.h"
#include "settinglib.h"
#include "settingmypage.h"
#include "settingrosemenu.h"
#include "settingrosetube.h"
#include "settingtrackoption.h"
#include "settinghdmi.h" //cheon29_src
#include "settingtimer.h" //cheon01_src
#include "settinglanguage.h" //cheon07_language
#include "settingvideo.h"
#include "settingvu.h"

#include <QWidget>
#include <QMutex>

#include <home/abstractmaincontent.h>
#include <music/albumdetail.h>
#include <music/artistinfo.h>
#include <music/metainfo.h>
#include <music/musice_playlist.h>
#include <music/musicfolder.h>
#include <music/musiclist_group.h>
#include <rosetube/rosetube_viewall.h>
#include <rosetube/rosetube_Playlist.h>


#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief The SettingMain class 설정 메인메뉴의 총괄
 */
class SettingMain : public AbstractMainContent
{
    Q_OBJECT
public:
    explicit SettingMain(QWidget *parent = nullptr);
    ~SettingMain();

    void changedPageVU();
    void changedPageInputOutput();
    void changedPageHDMI();
    void changedPageTimer();//cheon01_src
    void changedPageLanguage();////cheon07_language

signals:

private slots:
    void slot_clickedViewAll(QString p_viewAllMode);
    void slot_clickedViewAll(const QJsonObject &p_jsonData);

protected:
    void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

private :
    QMutex mutex;

    // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요
    LeftMenuBar *leftMenuBar;
    QScrollArea *scrollArea_leftMenuBar;

    // 메뉴 관련 페이지  ----------------------
    SettingMyPage *settingMyPage=nullptr;
    SettingFriendPage *settingFriendPage=nullptr;
    SettingLib *settingLib=nullptr;
    SettingDisplay *settingDisplay=nullptr;
    SettingTrackOption *settingTrackOption=nullptr;
    SettingCountry *settingCountry=nullptr;
    SettingRoseTube *settingRoseTube=nullptr;
    SettingVideo *settingVideo=nullptr;
    SettingHDMI *settingHDMI=nullptr;       //cheon29_src
    SettingTimer *settingTimer=nullptr;     //cheon01_src
    SettingLanguage *settingLanguage=nullptr;     //cheon07_language
    SettingVU *settingVU=nullptr;
    SettingInputOutput *settingInputOutput=nullptr;
    SettingRoseMenu *settingRoseMenu=nullptr;
    SettingAppMenu *settingAppMenu=nullptr;
    SettingAppInfo *settingAppInfo=nullptr;

    // 옵션팝업 관련 페이지 ----------------------
    AlbumDetail *albumDetail=nullptr;            ///< 옵션팝업 : 앨범상세
    Musice_playlist *musice_playlist=nullptr;    ///< 음악 : 플레이리스트 상세
    rosetube::RoseTube_PlayList *rosetube_playlist=nullptr;///< 로즈튜브 : 플레이리스트 상세
    MetaInfo *metaInfo=nullptr;                  ///< 옵션팝업 : 메타정보
    ArtistInfo *artistInfo=nullptr;              ///< 옵션팝업 : 아티스트상세

    // View All 관련 페이지 ----------------------
    MusicList_Group *viewAll_playListMusic=nullptr;     ///< 마이페이지에서 음악 플레이리스트의 ViewAll
    RoseTube_ViewAll *viewAll_playListRoseTube=nullptr; ///< 마이페이지에서 로즈튜브 플레이리스트의 ViewAll

    MusicFolder *settingMediaLibAddDel=nullptr;

    QWidget *wg_settingTotal;

    void resetLayoutUIfromParent();
    void setUIControl();


    void showPlayScreenBG() override;
    void hidePlayScreenBG() override;


};

#endif // SETTINGMAIN_H
