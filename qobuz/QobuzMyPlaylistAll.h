#ifndef QOBUZMYPLAYLISTALL_H
#define QOBUZMYPLAYLISTALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemPlaylist_forQobuz.h"
#include "qobuz/qobuz_struct.h"

#include "roseHome/ItemPlaylist_rosehome.h"

#include "bugs/BugsChooseFilterOpt.h"

#include "widget/FilterWidget.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 My Playlist 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class QobuzMyPlaylistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT

    public:
        explicit QobuzMyPlaylistAll(QWidget *parent = nullptr);
        ~QobuzMyPlaylistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;         ///< filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;
        void slot_applyResult_myCreatedPlaylists(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool flag_lastPage) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;


    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_filter();

        void request_more_playlistData();
        void request_more_playlistDraw();

    private:
        // UI
        FlowLayout *flowLayout_playlists;    ///< playlist's items

        // Data
        qobuz::ItemPlaylist_qobuz *listPlaylist_all[999999];

        QList<qobuz::PlaylistItemData> *list_playlist_created;

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
     * @brief Qobuz의 Recnetly Playlist 전체목록 화면
     */
    class QobuzRecentlyPlaylistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzRecentlyPlaylistAll(QWidget *parent = nullptr);
        ~QobuzRecentlyPlaylistAll();

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
        void slot_applyResult_playlistsCheck(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

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

        QString page = "";
        QString title = "";

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





    /**
     * @brief RoseHome의 플레이리스트 전체보기 화면 클래스
     */
    class QobuzPlaylistHistoryAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzPlaylistHistoryAll(QWidget *parent = nullptr);
        ~QobuzPlaylistHistoryAll();

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
     * @brief Qobuz의 My Playlist 전체목록 화면
     */
    class QobuzMyRosePlaylistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT

    public:
        explicit QobuzMyRosePlaylistAll(QWidget *parent = nullptr);
        ~QobuzMyRosePlaylistAll();

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





    /**
     * @brief Qobuz의 User Playlist 전체목록 화면
     */
    class QobuzUserRosePlaylistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT

    public:
        explicit QobuzUserRosePlaylistAll(QWidget *parent = nullptr);
        ~QobuzUserRosePlaylistAll();

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

#endif // QOBUZMYPLAYLISTALL_H
