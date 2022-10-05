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

        void applyLoginSuccessState() override;

    protected:
        void setUIControl_logined() override;
        void proc_preUpdate_favorite_pd_album(const ItemPositionData&) override;

    protected slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>&, const bool) override;

        void slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_list_theme_setDone();
        void slot_clicked_themeButton();

    private:
        bugs::ItemPD_Album_bugs *explore_recent[15];
        bugs::ItemPD_Album_bugs *explore_popular[15];



        bool flagReg_album_recent;
        bool flagReg_album_popular;
        bool flagReg_theme;

        QHBoxLayout *hBox_pd_album_recent;
        QHBoxLayout *hBox_pd_album_popular;

        QList<bugs::PD_AlbumItemData> *list_pd_album_recent;
        QList<bugs::PD_AlbumItemData> *list_pd_album_popular;

        FlowLayout *flowLayout_theme;
    };

};


#endif // BUGSEXPLORE_H
