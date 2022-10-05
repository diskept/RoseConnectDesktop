#ifndef TIDALTRACKSLISTALL_H
#define TIDALTRACKSLISTALL_H

#include "AbstractTidalSubWidget.h"
#include "delegate/TidalTrackDelegate.h"
#include <QListWidget>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal의 Track 전체보기 화면 클래스
     */
    class TidalTracksListAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalTracksListAll(QWidget *parent = nullptr);
        ~TidalTracksListAll();
        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const tidal::TidalTrackDelegate::ClickMode) override;

        void slot_thumbnailDownloaded_fotListWidget_track() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;
        void request_more_trackData();

        QString api_subPath;            ///< 타이달 API 요청을 위한 subPath

        QLabel *label_mainTitle;

        QListWidget *listWidget_track;
        TidalTrackDelegate *delegate;
        QList<tidal::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

    };

};


#endif // TIDALTRACKSLISTALL_H
