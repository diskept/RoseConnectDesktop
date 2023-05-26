#ifndef ROSETUBE_MYPLAYLISTALL_H
#define ROSETUBE_MYPLAYLISTALL_H

#include "rosetube/ItemTrack_rosetube.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"
#include "roseHome/ItemPlaylist_rosehome.h"

#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"

#include "widget/framerosetube.h"

#include <QCoreApplication>


namespace rosetube {

    class RoseTubePlaylistRecentlyAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTubePlaylistRecentlyAll(QWidget *parent = nullptr);
        ~RoseTubePlaylistRecentlyAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         ///< filter Box 사이즈 조절을 위해

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220818 share link
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_recentlyPlaylist_rose(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_playlistCheck(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_btnClicked_Delete();

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_filter();

        void request_more_playlistData();
        void request_more_playlistDraw();

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

        // Data
        roseHome::ItemPlaylist_rosehome *listPlaylist_all[999999];

        QList<roseHome::PlaylistItemData> *list_playlist;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page_reload_check = "";
        QString page_mainTitle = "";

        int playlist_widget_width = 0;
        int playlist_widget_margin = 0;

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





    class RoseTubeRecentlyPlaylistAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTubeRecentlyPlaylistAll(QWidget *parent = nullptr);
        ~RoseTubeRecentlyPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         ///< filter Box 사이즈 조절을 위

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220818 share link
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_myPlaylist_rose(const QJsonArray&, const int&, const bool);
        void slot_applyResult_rosetubelistCheck(const QJsonArray&, const int&, const bool);

        void slot_btnClicked_Delete();

        void slot_applyResult_getRating_track(const QJsonArray&);

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_filter();

        void request_more_playlistData();
        void request_more_playlistDraw();

        void initAll();

    private:
        // 관리 필요한 Layout UI
        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        //FrameRoseTube *recently_roseTube[999999];
        rosetube::ItemTrack_rosetube *recently_roseTube[999999];

        QJsonArray playlist_Arr;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                                ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page_reload_check = "";
        QString page_mainTitle = "";

        int playlist_widget_width = 0;
        int playlist_widget_margin = 0;

        int playlist_widget_cnt = 0;

        int next_offset = 0;
        int playlist_total_cnt = 0;
        int playlist_draw_cnt = 0;

        // getting more data
        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;

        bool flag_flow_draw = false;
        bool flag_playlist_draw = false;

        int flag_fav_idx = 0;
        int flag_fav_star = 0;
        int flag_fav_type = 0;

    };





    class RoseTubeHistoryPlaylistAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTubeHistoryPlaylistAll(QWidget *parent = nullptr);
        ~RoseTubeHistoryPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

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
        roseHome::ItemPlaylist_rosehome *widget_playlist[9999];

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





    /**
     * @brief RoseHome의 트랙 전체보기 화면 클래스
     */
    class RoseTubeHistoryTrackAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseTubeHistoryTrackAll(QWidget *parent = nullptr);
        ~RoseTubeHistoryTrackAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

        void resizeEvent(QResizeEvent *event) override;

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_historyTrack(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void setUIControl_tracks();

        void request_more_trackData();
        void request_more_trackDraw();

    private:
        rosetube::ItemTrack_rosetube *historyTrack[9999];

        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_tracks;

        QWidget *widget_filter_contents;

        // data
        QList<roseHome::TrackItemData> *list_track;

        roseHome::PageInfo_TrackAllView data_pageInfo;

        QJsonArray jsonArr_tracks_toPlay;

        int track_widget_width = 0;
        int track_widget_margin = 0;

        int track_widget_cnt = 0;

        int track_next_offset = 0;
        int track_totalCount = 0;
        int track_drawCount = 0;
        int track_favoriteOffset = 0;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_widget_visible = false;

        bool flag_draw = false;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };





    class RosetubeMyRosePlaylistAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RosetubeMyRosePlaylistAll(QWidget *parent = nullptr);
        ~RosetubeMyRosePlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;         ///< filter Box 사이즈 조절을 위해

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220818 share link
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_myPlaylist_rose(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_filter();

        void request_more_playlistData();
        void request_more_playlistDraw();

    private:
        // UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        roseHome::ItemPlaylist_rosehome *listPlaylist_all[999999];

        QList<roseHome::PlaylistItemData> *list_playlist;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page_reload_check = "";

        int playlist_widget_width = 0;
        int playlist_widget_margin = 0;

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





    class RosetubeUserRosePlaylistAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RosetubeUserRosePlaylistAll(QWidget *parent = nullptr);
        ~RosetubeUserRosePlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;         ///< filter Box 사이즈 조절을 위해

    signals:
        void signal_clickedViewAll(const QJsonObject &p_jsonObject);

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220818 share link
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_userPlaylist_rose(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_filter();

        void request_more_playlistData();
        void request_more_playlistDraw();

    private:
        // UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        roseHome::ItemPlaylist_rosehome *listPlaylist_all[999999];

        QList<roseHome::PlaylistItemData> *list_playlist;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page_reload_check = "";

        int playlist_widget_width = 0;
        int playlist_widget_margin = 0;

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
#endif // ROSETUBE_MYPLAYLISTALL_H
