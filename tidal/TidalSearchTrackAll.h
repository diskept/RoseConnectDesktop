#ifndef TIDALSEARCHTRACKALL_H
#define TIDALSEARCHTRACKALL_H

#include "AbstractTidalSubWidget.h"
#include "delegate/TidalTrackDelegate.h"
#include <QListWidget>
#include "common/common_struct.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief 검색결과 - Track 전체 목록 클래스
     */
    class TidalSearchTrackAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalSearchTrackAll(QWidget *parent = nullptr);
        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const tidal::TidalTrackDelegate::ClickMode) override;

        void slot_thumbnailDownloaded_fotListWidget_track() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:

        common::SearchData data_search;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;
        void request_more_trackData();

        QLabel *label_mainTitle;

        QListWidget *listWidget_track;
        TidalTrackDelegate *delegate;
        QList<tidal::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;

    };

};


#endif // TIDALSEARCHTRACKALL_H
