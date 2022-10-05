#ifndef TIDALSEARCHPLAYLISTALL_H
#define TIDALSEARCHPLAYLISTALL_H

#include "tidal/AbstractTidalSubWidget.h"
#include "tidal/ItemPlaylist.h"
#include "tidal/tidal_struct.h"

#include "common/common_struct.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief 검색결과 - Playlist 전체 목록 클래스
     */
    class TidalSearchPlaylistAll : public AbstractTidalSubWidget
    {
        Q_OBJECT

    public:
        explicit TidalSearchPlaylistAll(QWidget *parent = nullptr);
        ~TidalSearchPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        void request_more_playlistData();
        void request_more_playlistDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;

        common::SearchData data_search;

        tidal::ItemPlaylist *listPlaylist_all[999999];

        QList<tidal::PlaylistItemData> *list_playlist;

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
#endif // TIDALSEARCHPLAYLISTALL_H
