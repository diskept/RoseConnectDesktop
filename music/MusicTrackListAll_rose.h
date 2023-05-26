#ifndef MUSICTRACKLISTALL_ROSE_H
#define MUSICTRACKLISTALL_ROSE_H

#include "common/linker.h"
#include "common/common_struct.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace music {

    /**
     * @brief RoseHome의 트랙 전체보기 화면 클래스
     */
    class TrackListAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit TrackListAll(QWidget *parent = nullptr);
        ~TrackListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

        void resizeEvent(QResizeEvent *event) override;

    protected slots:
        void slot_applyResult_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_tracklistCheck(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

        void slot_btnClicked_Delete();

    private:
        void setUIControl_tracks();

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
     * @brief RoseHome의 트랙 전체보기 화면 클래스
     */
    class ArtistTrackListAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit ArtistTrackListAll(QWidget *parent = nullptr);
        ~ArtistTrackListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        void slot_applyResult_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_checkTrackId(const QJsonObject&);
        void slot_applyResult_getTrckRating(const QJsonArray&);
        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void setUIControl_tracks();
        void setDataTrackFromDB();

        void request_more_trackDraw();

        int get_rose_playType(OptMorePopup::ClickMode clickMode);
        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

    private:
        Linker *linker;

        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        // data
        QList<roseHome::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        QString title = "";
        QString artist = "";

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

        int track_svr_idx = 0;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };





    /**
     * @brief RoseHome의 트랙 전체보기 화면 클래스
     */
    class TrackHistoryAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit TrackHistoryAll(QWidget *parent = nullptr);
        ~TrackHistoryAll();

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

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

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
     * @brief Search Main의 트랙 전체보기 화면 클래스
     */
    class SearchTrackListAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit SearchTrackListAll(QWidget *parent = nullptr);
        ~SearchTrackListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        void slot_applyResult_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_checkTrackId(const QJsonObject&);
        void slot_applyResult_getTrckRating(const QJsonArray&);
        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void setUIControl_tracks();

        void request_more_trackDraw();

        int get_rose_playType(OptMorePopup::ClickMode clickMode);
        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

    private:
        Linker *linker;

        QLabel *label_mainTitle;
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        // data
        QList<roseHome::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        QString title = "";

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

        int track_svr_idx = 0;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };
};
#endif // MUSICTRACKLISTALL_ROSE_H
