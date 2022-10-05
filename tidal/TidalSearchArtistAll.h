#ifndef TIDALSEARCHARTISTALL_H
#define TIDALSEARCHARTISTALL_H

#include "AbstractTidalSubWidget.h"
#include "common/common_struct.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief 검색결과 - Artist 전체 목록 클래스
     */
    class TidalSearchArtistAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalSearchArtistAll(QWidget *parent = nullptr);
        ~TidalSearchArtistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;

        void slot_applyResult_artists(const QList<tidal::ArtistItemData>&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        void request_more_artistData();
        void request_more_artistDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_artists;

        common::SearchData data_search;

        // Data
        tidal::ItemArtist *listArtist_all[99999];

        QList<tidal::ArtistItemData> *list_artist;

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
#endif // TIDALSEARCHARTISTALL_H
