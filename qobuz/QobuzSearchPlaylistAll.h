#ifndef QOBUZSEARCHPLAYLISTALL_H
#define QOBUZSEARCHPLAYLISTALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemPlaylist_forQobuz.h"
#include "qobuz/qobuz_struct.h"

#include "common/common_struct.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief 검색결과 - Playlist 전체 목록 클래스
     */
    class QobuzSearchPlaylistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT

    public:
        explicit QobuzSearchPlaylistAll(QWidget *parent = nullptr);
        ~QobuzSearchPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_playlists(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();

        void request_more_playlistData();
        void request_more_playlistDraw();

    private:
        common::SearchData data_search;

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;

        // Data
        qobuz::ItemPlaylist_qobuz *listPlaylist_all[999999];

        QList<qobuz::PlaylistItemData> *list_playlist;

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
#endif // QOBUZSEARCHPLAYLISTALL_H
