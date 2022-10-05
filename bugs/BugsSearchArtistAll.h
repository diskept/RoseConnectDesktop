#ifndef BUGSSEARCHARTISTALL_H
#define BUGSSEARCHARTISTALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemArtist_bugs.h"

#include "common/common_struct.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief 검색결과 - Artist 전체 목록 클래스
     */
    class BugsSearchArtistAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsSearchArtistAll(QWidget *parent = nullptr);
        ~BugsSearchArtistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;

        void slot_applyResult_artists(const QList<bugs::ArtistItemData>&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_preUpdate_favorite_artist(const ItemPositionData&) override;
        void proc_wheelEvent_to_getMoreData() override;

    private slots:

    private:
        void setUIControl_artists();

        void request_more_artistData();
        void request_more_artistDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_artist;

        bugs::ItemArtist_bugs *list_artist_all[999999];

        // Data
        QList<bugs::ArtistItemData> *list_artist;

        QString page;
        common::SearchData data_search;

        int artist_widget_cnt = 0;

        int next_offset = 0;
        int artist_total_cnt = 0;
        int artist_draw_cnt = 0;

        bool flagReqMore_artist = false;
        bool flag_lastPage_artist = false;

        bool flag_flow_draw = false;
        bool flag_artist_draw = false;
    };

};

#endif // BUGSSEARCHARTISTALL_H
