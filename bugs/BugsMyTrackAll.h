#ifndef BUGSMYTRACKALL_H
#define BUGSMYTRACKALL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/BugsChooseFilterOpt.h"

#include "widget/FilterWidget.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief Tidal의 My Track 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class BugsRecentlyTrackAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsRecentlyTrackAll(QWidget *parent = nullptr);
        ~BugsRecentlyTrackAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_myTracks_rose(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_tracklistCheck(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_btnClicked_Delete();

        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();
        void request_more_trackDraw();

        void initAll();

    private:
        Linker *linker;

        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        // data
        QList<roseHome::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        QString page;
        QString title;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_widget_visible = false;

        bool flag_draw = false;
        int next_offset = 0;
        int track_totalCount = 0;
        int track_drawCount = 0;
        int track_favoriteOffset = 0;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };





    /**
     * @brief RoseHome의 트랙 전체보기 화면 클래스
     */
    class BugsTrackHistoryAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsTrackHistoryAll(QWidget *parent = nullptr);
        ~BugsTrackHistoryAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_historyTrack(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_btnClicked_playAll_rose();
        void slot_btnClicked_playShuffle_rose();

    private:
        void setUIControl_tracks();

        void request_more_trackData();
        void request_more_trackDraw();

    private:
        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        // data
        QList<roseHome::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        roseHome::PageInfo_TrackAllView data_pageInfo;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_widget_visible = false;

        bool flag_draw = false;

        int next_offset = 0;
        int track_totalCount = 0;
        int track_drawCount = 0;
        int track_favoriteOffset = 0;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };





    /**
     * @brief Tidal의 My Track 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class BugsTrackRecentlyAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsTrackRecentlyAll(QWidget *parent = nullptr);
        ~BugsTrackRecentlyAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_trackRecently(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_listwidgetItemPressed(QListWidgetItem *item);

        void slot_clicked_btnVideolist();
        void slot_clicked_btnFavorite();
        void slot_clicked_btnMenu();

        void slot_thumbnailDownload_list();

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();
        void request_more_trackDraw();

        inline void iniDataInfo_RoseList_Item(const bugs::TrackItemData &data_track, const int &p_index);
        inline void setDataInfo_RoseList_Item(const bugs::TrackItemData &data_track, const int &p_index);

        void initAll();

    private:
        QLabel *label_mainTitle;

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QListWidget *listWidget_contents;

        QLabel *lb_thumb[999999];
        QLabel *lb_explicit[999999];
        QLabel *lb_title[999999];
        QLabel *lb_album[999999];
        QLabel *lb_resolution[999999];
        QLabel *lb_artist[999999];
        QLabel *lb_duration[999999];

        QPushButton *btn_list_fav[9999999];

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        // data
        QList<bugs::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        QString page;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_widget_visible = false;

        bool flag_draw = false;
        int next_offset = 0;
        int track_totalCount = 0;
        int track_drawCount = 0;
        int track_favoriteOffset = 0;
        int before_scrollMax = 0;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav[9999999];
    };





    /**
     * @brief Tidal의 My Track 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class BugsTrackChartAll : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsTrackChartAll(QWidget *parent = nullptr);
        ~BugsTrackChartAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_btnClicked_playAll() override;
        void slot_btnClicked_playShuffle() override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_trackRecently(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();
        void request_more_trackDraw();

        void initAll();

    private:
        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        // data
        QList<bugs::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        QString page;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_widget_visible = false;

        bool flag_draw = false;
        int next_offset = 0;
        int track_totalCount = 0;
        int track_drawCount = 0;
        int track_favoriteOffset = 0;
        int before_scrollMax = 0;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };
};

#endif // BUGSMYTRACKALL_H
