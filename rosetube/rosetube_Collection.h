#ifndef ROSETUBE_COLLECTION_H
#define ROSETUBE_COLLECTION_H

#include "rosetube/ItemTrack_rosetube.h"
#include "rosetube/ItemPlaylist_rosetube.h"

#include "common/linker.h"

#include "login/dialoglogin.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"
#include "roseHome/ItemPlaylist_rosehome.h"

#include <QCoreApplication>


namespace rosetube {

    /**
     * @brief 濡쒖쫰쒕툕 > My collection @n
     *      몃옓, My 뚮젅대━ㅽ듃, 移쒓뎄 뚮젅대━ㅽ듃濡援ъ꽦 @n
     *      媛ぉ 理쒖냼 媛닔留Show, 媛ぉ View All 대┃ 꾩껜 紐⑸줉 Show
     */
    class RoseTube_Collection : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseTube_Collection(QWidget * parent = nullptr);
        ~RoseTube_Collection();

        void setJsonObject_forData(const QJsonObject&) override;
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
        void slot_applyResult_getShareLink(const QString &link);//c220828_2

        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);
        void slot_change_cache_state();
        void slot_change_cache_list();

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_MyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_favoriteTracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_MyPlaylistCheck(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_favoriteTracksCheck(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_track(const QJsonArray&);

    private:
        // 초기 UI화면 구성
        void setUIControl_requestRose();

        void setUIControl_appendWidget_rose();
        void setUIControl_checkWidget_rose();

        void setUIControl_appendWidget_Delete();

        QWidget* setUIControl_LoginBefore();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);

        int get_rose_playType(OptMorePopup::ClickMode clickMode);

        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

    private:
        Linker *linker;

        Dialog::DialogLogin *dlg_rose_login;

        QString page = "";

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        roseHome::ItemPlaylist_rosehome *collection_myPlaylist[15];
        rosetube::ItemTrack_rosetube *collection_myFavorite[15];
        rosetube::ItemTrack_rosetube *collection_myCaching[15];

        QVBoxLayout *box_rose_contents;
        QVBoxLayout *box_login_contents;

        QWidget *widget_rose_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        QWidget *widget_myPlaylist;
        QWidget *widget_myFavorite;
        QWidget *widget_myCaching;

        QVBoxLayout *vBox_myPlaylist;
        QVBoxLayout *vBox_myFavorite;
        QVBoxLayout *vBox_myCaching;

        QHBoxLayout *hBox_myPlaylist;
        QHBoxLayout *hBox_myFavorite;
        QHBoxLayout *hBox_myCaching;

        // data
        QList<roseHome::PlaylistItemData> *list_MyPlaylist;
        QList<roseHome::TrackItemData> *list_MyFavorite;

        QJsonArray jsonArr_myFavorite;
        QJsonArray jsonArr_myCaching;
        QJsonArray jsonArr_myCachLock;
        QJsonObject jsonObj_CacheLock;

        int cache_lock_index = 0;
        bool cache_lock_state = false;
        bool flag_cache_lock_send = false;
        bool flag_cache_delete_send = false;

        bool flag_myFavorite[2] = {false, false};
        bool flag_myPlaylist[2] = {false, false};
        bool flag_myCaching[2] = {false, false};

        bool flag_myFavorite_check[2] = {false, false};
        bool flag_myPlaylist_check[2] = {false, false};

        int flag_fav_idx = 0;
        int flag_fav_star = 0;
        int flag_fav_type = 0;

        bool flag_page_reload = false;
    };
};

#endif // ROSETUBE_COLLECTION_H
