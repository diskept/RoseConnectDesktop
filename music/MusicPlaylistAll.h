#ifndef MUSICPLAYLISTALL_H
#define MUSICPLAYLISTALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemPlaylist_rosehome.h"

#include <QCoreApplication>


namespace music {

    /**
     * @brief RoseHome의 플레이리스트 전체보기 화면 클래스
     */
    class PlaylistAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit PlaylistAll(QWidget *parent = nullptr);
        ~PlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;

    protected slots:
        void slot_applyResult_playlists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_playlistCheck(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_btnClicked_Delete();

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();

        void request_more_Data();
        void request_more_Draw();

        void initAll();

    private:
        Linker *linker;

        // 관리 필요한 Layout UI
        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        FlowLayout *flowLayout_playlists;    ///< playlist's items

        roseHome::ItemPlaylist_rosehome *widget_playlist[99999];

        // Data
        QList<roseHome::PlaylistItemData> *list_playlist;

        QString curr_api_subPath;
        QString curr_api_mainTitle;

        int playlist_widget_width = 0;
        int playlist_widget_margin = 0;

        int playlist_widget_cnt = 0;

        int playlist_next_offset = 0;
        int playlist_totalCount = 0;
        int playlist_drawCount = 0;

        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;

        bool flag_flow_draw = false;
        bool flag_playlist_draw = false;
    };





    /**
     * @brief RoseHome의 플레이리스트 전체보기 화면 클래스
     */
    class MyPlaylistAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit MyPlaylistAll(QWidget *parent = nullptr);
        ~MyPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;

    protected slots:
        void slot_applyResult_playlists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();

        void request_more_Data();
        void request_more_Draw();

    private:
        Linker *linker;

        roseHome::ItemPlaylist_rosehome *widget_playlist[99999];

        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        QList<roseHome::PlaylistItemData> *list_playlist;

        QString curr_api_subPath;
        QString curr_api_mainTitle;

        int playlist_widget_width = 0;
        int playlist_widget_margin = 0;

        int playlist_widget_cnt = 0;

        int playlist_next_offset = 0;
        int playlist_totalCount = 0;
        int playlist_drawCount = 0;

        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;

        bool flag_flow_draw = false;
        bool flag_playlist_draw = false;
    };





    /**
     * @brief RoseHome의 플레이리스트 전체보기 화면 클래스
     */
    class UserPlaylistAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit UserPlaylistAll(QWidget *parent = nullptr);
        ~UserPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_playlists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();

        void request_more_Data();
        void request_more_Draw();

    private:
        Linker *linker;

        roseHome::ItemPlaylist_rosehome *widget_playlist[99999];

        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        QList<roseHome::PlaylistItemData> *list_playlist;

        QString curr_api_subPath;
        QString curr_api_mainTitle;

        int playlist_next_offset = 0;
        int playlist_totalCount = 0;
        int playlist_drawCount = 0;

        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;

        bool flag_flow_draw = false;
        bool flag_playlist_draw = false;
    };




    /**
     * @brief RoseHome의 플레이리스트 전체보기 화면 클래스
     */
    class PlaylistHistoryAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit PlaylistHistoryAll(QWidget *parent = nullptr);
        ~PlaylistHistoryAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_historyPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);


    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();

        void request_more_Data();
        void request_more_Draw();

    private:
        roseHome::ItemPlaylist_rosehome *widget_playlist[99999];

        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        QList<roseHome::PlaylistItemData> *list_playlist;

        roseHome::PageInfo_PlaylistAllView data_pageInfo;

        int playlist_widget_width = 0;
        int playlist_widget_margin = 0;

        int playlist_widget_cnt = 0;

        int playlist_next_offset = 0;
        int playlist_totalCount = 0;
        int playlist_drawCount = 0;

        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;

        bool flag_flow_draw = false;
        bool flag_playlist_draw = false;
    };
};

#endif // MUSICPLAYLISTALL_H
