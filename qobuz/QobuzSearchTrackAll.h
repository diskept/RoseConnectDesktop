#ifndef QOBUZSEARCHTRACKALL_H
#define QOBUZSEARCHTRACKALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include "common/common_struct.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief 검색결과 - Track 전체 목록 클래스
     */
    class QobuzSearchTrackAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzSearchTrackAll(QWidget *parent = nullptr);
        ~QobuzSearchTrackAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_checkComplete_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_btnClicked_playAll();
        void slot_btnClicked_playShuffle();

    private:
        void request_more_trackData();
        void request_more_trackDraw();

    private:
        common::SearchData data_search;

        QLabel *label_mainTitle;

        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        QList<qobuz::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;

        bool flag_widget_visible = false;

        bool flag_draw = false;
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


#endif // QOBUZSEARCHTRACKALL_H
