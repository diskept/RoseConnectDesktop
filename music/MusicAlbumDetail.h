#ifndef MUSICALBUMDETAIL_H
#define MUSICALBUMDETAIL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#include <QCoreApplication>


namespace music {

    /**
     * @brief RecentAlbum의 Album 상세보기 화면
     */
    class AlbumDetail : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit AlbumDetail(QWidget *parent = nullptr);
        ~AlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        // albumImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_changedSubTabUI_Album(const QJsonObject &p_data);
        void changedOnlyTabUI_notSendSignal(QString p_step);

        void slot_applyResult_album(const QJsonArray&);
        void slot_applyResult_album_tracks(const QJsonArray&);
        void slot_applyResult_rose_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_checkRating_album(const QJsonObject&);
        void slot_applyResult_checkRating_track(const QJsonObject&);

        void slot_applyResult_addRating_album(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_applyResult_getRating_album(const QJsonArray&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_add_rosePlaylist_withMusic(const int&, const QJsonObject&);

    private:
        void request_more_trackData();
        void request_more_trackDraw();

        QJsonArray getDataForPlayMusic(const int);
        QJsonArray reorderJsonArray(const QJsonArray&, const int);

    private:
        Linker *linker;

        DialogNotice *dlgAlbumInfo;

        AbstractImageDetailContents_RHV *album_detail_info;
        AlbumTrackDetailInfo_RHV *album_track_info[99999];

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vbox_trackList;

        TopMenuBar *menubar;

        QStackedWidget *stackedwidget;
        QWidget *widget_tab_tracks;
        QWidget *widget_tab_credit;

        QVBoxLayout *vl_tracks;
        QVBoxLayout *vl_credit;

        // data
        int albumId = 0;
        QString albumKey = "";
        QString albumPath = "";
        QString albumImg = "";
        QString albumTitle = "";
        QString albumMime = "";
        QString artistName = "";
        QString albumDiscription = "";
        int albumDuration = 0;

        QList<QString> trackID;
        QJsonArray jsonArr_tracks_toPlay;                       ///< Track 전체를 재생하기 위함

        QString contentStep_Album;

        bool flag_draw = false;

        int cd_ico_cnt = 0;
        int cd_num = 0;

        int album_id_roseSVR = 0;
        int track_page = 0;

        int track_drawCount = 0;
        int track_totalCount = 0;
        int track_favoriteOffset = 0;

        int track_id_fav = 0;
        int track_idx_fav = 0;

        int album_star_fav = 0;
        int track_star_fav = 0;

        bool flag_album_fav = false;
        bool flag_track_fav = false;
        bool flag_album_thumb = false;

        bool flag_check_album = false;
        bool flag_check_track = false;

        bool flag_track_ok = false;

        QList<roseHome::TrackItemData> *list_track;
    };
};

#endif // MUSICALBUMDETAIL_H
