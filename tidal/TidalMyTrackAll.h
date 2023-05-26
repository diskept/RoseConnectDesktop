#ifndef TIDALMYTRACKALL_H
#define TIDALMYTRACKALL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "bugs/BugsChooseFilterOpt.h"

#include "widget/FilterWidget.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal의 My Track 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class TidalMyTrackAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalMyTrackAll(QWidget *parent = nullptr);
        ~TidalMyTrackAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)
        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_applyResult_myFavoriteTracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();
        void request_more_trackDraw();

        void initAll();

    private:
        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;          ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;       // nullptr 초기 필수

        // data
        QList<tidal::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;                           ///< Track 전체를 재생하기 위함
        QString page;                                        ///< 타이달 API 요청을 위한 subPat

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
    class TidalRecentlyTrackAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalRecentlyTrackAll(QWidget *parent = nullptr);
        ~TidalRecentlyTrackAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
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

        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

        void slot_btnClicked_Delete();

    private:
        void setUIControl_tracks();
        void setUIControl_filter();

        void request_more_trackData();
        void request_more_trackDraw();

        void initAll();

    private:
        Linker *linker;

        // 관리 필요한 Layout UI
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
    class TidalTrackHistoryAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalTrackHistoryAll(QWidget *parent = nullptr);
        ~TidalTrackHistoryAll();

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

        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

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
};

#endif // TIDALMYTRACKALL_H
