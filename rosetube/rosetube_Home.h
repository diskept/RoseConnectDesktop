#ifndef ROSETUBEHOME_H
#define ROSETUBEHOME_H

#include "rosetube/ItemTrack_rosetube.h"
#include "rosetube/ItemPlaylist_rosetube.h"

#include "common/linker.h"

#include "login/dialoglogin.h"
//#include "widget/customlineedit.h"//c220728
#include "widget/myqwidget.h"//c220727

#include "roseHome/AbstractRoseHomeSubWidget.h"
#include "roseHome/ItemPlaylist_rosehome.h"


/**
 * @brief 로즈튜브-홈 @n
 *      최근재생로즈튜브, My 플레이리스트, 친구 플레이리스트, 로즈튜브 TOP 10, 로즈튜브 추천리스트로 구성 @n
 *      각 항목 최소 갯수 Show, 각 항목 View All 클릭시 전체 목록 Show
 *
 */
namespace rosetube {

    class RoseTubeHome : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTubeHome(QWidget *parent = nullptr);
        ~RoseTubeHome();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

        // about Item
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:

        void slot_optMorePopup_menuClicked22(int id);//c220818

        void slot_time_out();
        void slot_hide_msg();

        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_recentlyRosetube(const QJsonArray&, const int&, const bool);
        void slot_applyResult_MyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_UserPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_subCategories(const QJsonArray&);
        void slot_applyResult_getShareLink(const QString &link);//c220818 share link

    private :
        // 초기 UI화면 구성
        void setUIControl_requestRose();
        void setUIControl_requestRosetube();

        void setUIControl_appendWidget_rose();

        QWidget* setUIControl_LoginBefore();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        //void make_CustomLineEdit();//c220728

    private:
        Linker *linker;

        Dialog::DialogLogin *dlg_rose_login;

        QString page = "";

        QLabel *lb_subTitle[30];
        QPushButton *btnView_all[30];

        //==============================================================================
        QStackedWidget *stackRecent;

        rosetube::ItemTrack_rosetube *home_recently_rosetube[15];
        roseHome::ItemPlaylist_rosehome *home_myPlaylist[15];
        roseHome::ItemPlaylist_rosehome *home_userPlaylist[15];

        QVBoxLayout *box_rose_contents;
        QVBoxLayout *box_login_contents;

        QWidget *widget_rose_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        QWidget *widget_recentlyRoseTube;
        QWidget *widget_myPlaylist;
        QWidget *widget_userPlaylist;

        QVBoxLayout *vBox_recentlyRoseTube;
        QVBoxLayout *vBox_myPlaylist;
        QVBoxLayout *vBox_userPlaylist;

        QHBoxLayout *hBox_recentlyRoseTube;
        QHBoxLayout *hBox_myPlaylist;
        QHBoxLayout *hBox_userPlaylist;

        // data
        QList<roseHome::PlaylistItemData> *list_MyPlaylist;
        QList<roseHome::PlaylistItemData> *list_UserPlaylist;

        QJsonArray jsonArr_rosetube;
        QJsonArray jsonArr_track[10];

        int recentRoseTube_cnt = 0;

        bool flag_recentRosetube[2] = {false, false};
        bool flag_myPlaylist[2] = {false, false};
        bool flag_userPlaylist[2] = {false, false};

        //==============================================================================
        rosetube::ItemTrack_rosetube *home_rosetube_track[10][10];
        rosetube::ItemPlaylist_rosetube *home_rosetube_playlist[5][10];

        QWidget *widget_roseTube_Track[10];
        QWidget *widget_roseTube_Playlist[5];

        QVBoxLayout *vBox_Track[10];
        QVBoxLayout *vBox_Playlist[5];

        QHBoxLayout *hBox_Track[10];
        QHBoxLayout *hBox_Playlist[5];

        bool flag_track[10];
        bool flag_playlist[5];

        int flag_track_idx = 0;
        int flag_playlist_idx = 0;

        QJsonArray rosetubeCategory;
        QJsonArray jsonArr_rosetubeTrack[10];
        QJsonArray jsonArr_rosetubePlaylist[5];

        bool flag_Rosetube[2] = {false, false};
    };
};

#endif // ROSETUBEHOME_H

