#ifndef TIDALMYTIDALPLAYLISTALL_H
#define TIDALMYTIDALPLAYLISTALL_H

#include "AbstractTidalSubWidget.h"
#include "tidal_struct.h"
#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief TidalMy Playlist 꾩껜紐⑸줉 붾㈃ ("醫뗭븘 좏깮紐⑸줉 꾩껜)
     */
    class TidalMyTidalPlaylistAll : public AbstractTidalSubWidget
    {
        Q_OBJECT

    public:
        explicit TidalMyTidalPlaylistAll(QWidget *parent = nullptr);
        ~TidalMyTidalPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode) override;
        //void slot_applyResult_myCreatedPlaylists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool flag_lastPage) override;
        void slot_applyResult_myFavoritePlaylists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool flag_lastPage) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        // My created playlist 처리에 대해서 UI갱신 처리
        void proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData&) override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:

        void setUIControl_basic();
        void setUIControl_filter();

        void initAll();

        // getting data
        void request_more_playlistData();
        void request_more_playlistDraw();

        // UI
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 꾪꽣留諛뺤뒪꾩슱 곗씠 group 1媛쒕컰놁쓬
        QVariant selected_filterCode;                               ///< 꾩옱 붾㈃먯꽌 좏깮꾪꽣留肄붾뱶
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 珥덇린 꾩닔

        // Data
        tidal::ItemPlaylist *listPlaylist_all[999999];

        QList<tidal::PlaylistItemData> *list_playlist_tidal;

        QString api_subPath;

        int playlist_widget_cnt = 0;

        int next_offset = 0;
        int playlist_total_cnt = 0;
        int playlist_draw_cnt = 0;

        // getting more data
        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;

        bool flag_flow_draw = false;
        bool flag_playlist_draw = false;
    };

};

#endif // TIDALMYTIDALPLAYLISTALL_H
