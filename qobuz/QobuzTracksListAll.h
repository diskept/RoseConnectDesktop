#ifndef QOBUZTRACKSLISTALL_H
#define QOBUZTRACKSLISTALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include "delegate/QobuzTrackDelegate.h"

#include <QListWidget>
#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 Track 전체보기 화면 클래스
     */
    class QobuzTracksListAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzTracksListAll(QWidget *parent = nullptr);
        ~QobuzTracksListAll();
        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemTrack_inList(const int, const qobuz::QobuzTrackDelegate::ClickMode) override;

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
        QobuzTrackDelegate *delegate;
        QList<qobuz::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

    };

    /**
     * @brief Qobuz의 Track 전체보기 화면 클래스
     */
    class QobuzAppearsonListAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzAppearsonListAll(QWidget *parent = nullptr);
        ~QobuzAppearsonListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

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

        PlaylistTrackDetailInfo_RHV *track_listAll[999999];

        QWidget *widget_filter_contents;

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vBox_tracks;

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;        

        QString api_subPath;            ///< 타이달 API 요청을 위한 subPath
        QString api_subExtra;
        int artist_id;

        QLabel *label_mainTitle;

        QList<qobuz::TrackItemData> *list_track;
        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

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


#endif // QOBUZTRACKSLISTALL_H
