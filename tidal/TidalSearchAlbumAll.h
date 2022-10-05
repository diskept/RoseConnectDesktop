#ifndef TIDALSEARCHALBUMALL_H
#define TIDALSEARCHALBUMALL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"

#include "common/common_struct.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief 검색결과 - Album 전체 목록 클래스
     */
    class TidalSearchAlbumAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalSearchAlbumAll(QWidget *parent = nullptr);
        ~TidalSearchAlbumAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        void request_more_albumData();
        void request_more_albumDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;

        // Data
        tidal::ItemAlbum *listAlbum_all[999999];

        QList<tidal::AlbumItemData> *list_album;

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

#endif // TIDALSEARCHALBUMALL_H
