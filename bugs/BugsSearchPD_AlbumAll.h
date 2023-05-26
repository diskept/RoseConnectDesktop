#ifndef BUGSSEARCHPD_ALBUMALL_H
#define BUGSSEARCHPD_ALBUMALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemPD_Album_bugs.h"

#include "common/common_struct.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief 검색결과 - PD앨범 전체 목록 클래스
     */
    class BugsSearchPD_AlbumAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsSearchPD_AlbumAll(QWidget *parent = nullptr);
        ~BugsSearchPD_AlbumAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;   //c230223

    protected slots:
        void slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode) override;

        void slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_preUpdate_favorite_pd_album(const ItemPositionData&) override;
        void proc_wheelEvent_to_getMoreData() override;

    private slots:

    private:
        void setUIControl_PDalbums();

        void request_more_pd_albumData();
        void request_more_pd_albumDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_pd_albums;    ///< album's items

        bugs::ItemPD_Album_bugs *list_pd_album_all[999999];

        // Data
        QList<bugs::PD_AlbumItemData> *list_pd_album;

        QString page;
        common::SearchData data_search;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_pd_album = false;
        bool flag_lastPage_pd_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };

};


#endif // BUGSSEARCHPD_ALBUMALL_H
