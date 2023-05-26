#ifndef TIDALPLAYLISTALL_H
#define TIDALPLAYLISTALL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemPlaylist.h"

#include "tidal_struct.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal의 Playlist 전체보기 화면 클래스
     */
    class TidalPlaylistAll : public AbstractTidalSubWidget
    {
        Q_OBJECT

    public:
        explicit TidalPlaylistAll(QWidget *parent = nullptr);
        ~TidalPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해//c230223

    protected slots:
        void slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool) override;

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();

        void request_more_playlistData();
        void request_more_playlistDraw();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
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

#endif // TIDALPLAYLISTALL_H
