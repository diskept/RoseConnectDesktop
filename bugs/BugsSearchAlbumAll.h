#ifndef BUGSSEARCHALBUMALL_H
#define BUGSSEARCHALBUMALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemAlbum_bugs.h"

#include "common/common_struct.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief 검색결과 - Album 전체 목록 클래스
     */
    class BugsSearchAlbumAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsSearchAlbumAll(QWidget *parent = nullptr);
        ~BugsSearchAlbumAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        void slot_applyResult_albums(const QList<bugs::AlbumItemData>&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_preUpdate_favorite_album(const ItemPositionData&) override;
        void proc_wheelEvent_to_getMoreData() override;

    private slots:

    private:
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        bugs::ItemAlbum_bugs *listAlbum_all[999999];

        QList<bugs::AlbumItemData> *list_album;

        QString page;
        common::SearchData data_search;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };

};

#endif // BUGSSEARCHALBUMALL_H
