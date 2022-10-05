#ifndef ROSETUBEEXPLORE_H
#define ROSETUBEEXPLORE_H

#include "rosetube/ItemTrack_rosetube.h"

#include "login/dialoglogin.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"


namespace rosetube {

    /**
     * @brief 로즈튜브-탐색 @n
     *      장르 목록, 상황 목록, 2가지 추천 영상 목록으로 구성
     *
     * @note 디자인 시안과 장르/상황 목록및 영상 내용 다름.
     */
    class RoseTubeExplore : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTubeExplore(QWidget *parent = nullptr);
        ~RoseTubeExplore();

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
        void slot_hide_msg();
        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_clickedCategory();

    private:
        void slot_applyResult_getShareLink(const QString &link);//c220818
        // 초기 UI화면 구성
        void setUIControl_requestCategory();

        void appendCategory(const int &p_index, const QJsonObject &p_jsonObject);
        void appendCategorySelectorUI(const int &p_index, const QString &p_title, const QJsonArray &p_jsonArray);

        void setUIControl_requestExplore();

        void slot_applyResult_subCategories(const QJsonArray&);

        QWidget* setUIControl_LoginBefore();
        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);

    private:
        Linker *linker;

        Dialog::DialogLogin *dlg_rose_login;

        QString page = "";

        QLabel *lb_subTitle[20];
        QPushButton *btnView_all[20];

        rosetube::ItemTrack_rosetube *home_rosetube_track[20][10];

        QVBoxLayout *box_explore_contents;
        QVBoxLayout *box_login_contents;

        QWidget *widget_explore_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        QWidget *widget_roseTube_Track[20];

        QVBoxLayout *vBox_Track[20];
        QHBoxLayout *hBox_Track[20];

        QVBoxLayout *vBox_Explore;

        int flag_track_idx = 0;

        QJsonArray rosetubeCategory;
        QJsonArray jsonArr_rosetubeTrack[20];

    };
};

#endif // ROSETUBEEXPLORE_H
