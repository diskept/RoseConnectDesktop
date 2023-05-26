#ifndef BUGSEXPLORE_H
#define BUGSEXPLORE_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemPD_Album_bugs.h"

#include <QWidget>


namespace bugs {

    /**
     * @brief 벅스 > 탐색 첫 페이지 클래스
     */
    class BugsExplore : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsExplore(QWidget *parent = nullptr);
        ~BugsExplore();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;


    protected slots:
        void slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>&, const bool) override;

        void slot_list_theme_setDone();
        void slot_clicked_themeButton();


    private:
        void setUIControl_request();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout);
    private:
        QString page = "";

        QLabel *lb_subTitle[20];
        QPushButton *btnView_all[20];

        //==================================================

        bugs::ItemPD_Album_bugs *explore_recent[10];
        bugs::ItemPD_Album_bugs *explore_popular[10];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_pd_album_recent;
        QWidget *widget_pd_album_popular;
        QWidget *widget_theme;

        FlowLayout *flowLayout_theme;

        QVBoxLayout *vBox_pd_album_recent;
        QVBoxLayout *vBox_pd_album_popular;
        QVBoxLayout *vBox_theme;

        QHBoxLayout *hBox_pd_album_recent;
        QHBoxLayout *hBox_pd_album_popular;

        QList<bugs::PD_AlbumItemData> *list_pd_album_recent;
        QList<bugs::PD_AlbumItemData> *list_pd_album_popular;

        bool flagReg_album_recent[2] = {false, false};
        bool flagReg_album_popular[2] = {false, false};
        bool flagReg_theme[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };

};


#endif // BUGSEXPLORE_H
