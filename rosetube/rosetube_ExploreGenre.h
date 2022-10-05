#ifndef ROSETUBE_EXPLOREGENREALL_H
#define ROSETUBE_EXPLOREGENREALL_H

#include "login/dialoglogin.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "rosetube/ItemTrack_rosetube.h"
#include "rosetube/ItemPlaylist_rosetube.h"

#include <QCoreApplication>


namespace rosetube {

    /**
     * @brief 로즈튜브-탐색 @n
     *      장르 목록, 상황 목록, 2가지 추천 영상 목록으로 구성
     *
     * @note 디자인 시안과 장르/상황 목록및 영상 내용 다름.
     */
    class RoseTubeExploreGenre : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTubeExploreGenre(QWidget *parent = nullptr);
        ~RoseTubeExploreGenre();

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


    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220818
        void slot_hide_msg();
        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

    private:
        // 초기 UI화면 구성
        void setUIControl_requestGenre();
        void setUIControl_drawGenre();

        void slot_applyResult_subGenre(const QJsonArray&);

        QWidget* setUIControl_LoginBefore();
        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);

    private:
        Linker *linker;

        Dialog::DialogLogin *dlg_rose_login;

        QString page = "";

        QLabel *label_mainTitle;

        QLabel *lb_subTitle[50];
        QPushButton *btnView_all[50];

        rosetube::ItemTrack_rosetube *home_rosetube_track[50][10];
        rosetube::ItemPlaylist_rosetube *home_rosetube_playlist[50][10];

        QVBoxLayout *box_explore_contents;
        QVBoxLayout *box_login_contents;

        QWidget *widget_explore_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        QWidget *widget_roseTube_Track[50];

        QVBoxLayout *vBox_Track[50];
        QHBoxLayout *hBox_Track[50];

        QJsonArray rosetubeCategory;
        QJsonArray jsonArr_rosetubeTrack[50];

        int flag_track_idx = 0;

        int typeId;
        QString typeName;
        QString typeSubName;

        bool flag_draw = false;

        int genre_totalCount = 0;
        int genre_drawCount = 0;
    };
};

#endif // ROSETUBE_EXPLOREGENREALL_H
