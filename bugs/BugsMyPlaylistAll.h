#ifndef BUGSMYPLAYLISTALL_H
#define BUGSMYPLAYLISTALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "roseHome/ItemPlaylist_rosehome.h"

#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief Qobuz의 Recnetly Playlist 전체목록 화면
     */
    class BugsRecentlyPlaylistAll : public AbstractBugsSubWidget
    {
        Q_OBJECT

    public:
        explicit BugsRecentlyPlaylistAll(QWidget *parent = nullptr);
        ~BugsRecentlyPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;                         ///< filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;


    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_myPlaylist_rose(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUiControl_filter();

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
        QVariant selected_filterCode;                                ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        QString page_reload_check = "";

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





    /**
     * @brief Bugs의 My Playlist 전체목록 화면
     */
    class BugsMyRosePlaylistAll : public AbstractBugsSubWidget
    {
        Q_OBJECT

    public:
        explicit BugsMyRosePlaylistAll(QWidget *parent = nullptr);
        ~BugsMyRosePlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;         ///< filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;


    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_myPlaylist_rose(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUiControl_filter();

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





    /**
     * @brief Bugs의 User Playlist 전체목록 화면
     */
    class BugsUserRosePlaylistAll : public AbstractBugsSubWidget
    {
        Q_OBJECT

    public:
        explicit BugsUserRosePlaylistAll(QWidget *parent = nullptr);
        ~BugsUserRosePlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;         ///< filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;


    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_userPlaylist_rose(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUiControl_filter();

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

#endif // BUGSMYPLAYLISTALL_H
