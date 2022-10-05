#ifndef QOBUZPLAYLISTALL_H
#define QOBUZPLAYLISTALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemPlaylist_forQobuz.h"
#include "qobuz/qobuz_struct.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 Playlist 전체보기 화면 클래스
     */
    class QobuzPlaylistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT

    public:
        explicit QobuzPlaylistAll(QWidget *parent = nullptr);
        ~QobuzPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

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
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        qobuz::ItemPlaylist_qobuz *listPlaylist_all[999999];

        QList<qobuz::PlaylistItemData> *list_playlist;

        QString api_subPath;
        QString api_mainTitle;
        QString type;
        QString genre_ids;

        int genre_id;

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





    class QobuzSimilarplaylistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT

    public:
        explicit QobuzSimilarplaylistAll(QWidget *parent = nullptr);
        ~QobuzSimilarplaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_playlists(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();

    private:
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        qobuz::ItemPlaylist_qobuz *listPlaylist_all[999999];

        QList<qobuz::PlaylistItemData> *list_playlist;

        QString api_subPath;
        QString api_mainTitle;
        QString type;
        QString genre_ids;

        int genre_id;
    };

};

#endif // QOBUZPLAYLISTALL_H
