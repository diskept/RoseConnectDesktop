#ifndef APPLEALBUMDETAIL_H
#define APPLEALBUMDETAIL_H

#include "apple/Apple_struct.h"
#include "apple/AppleSettings.h"

#include "home/topmenubar.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace apple{

    /**
     * @brief APPLEMUSIC의 Album 상세화면 클래스
     */
    class AppleAlbumDetail : public AbstractAppleSubWidget
    {
        Q_OBJECT
    public:
        explicit AppleAlbumDetail(QWidget *parent = nullptr);
        ~AppleAlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        // albumImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

        void slot_changedSubTabUI(const QJsonObject &p_data) override;

    private slots:
        void slot_applyResult_album(const QJsonObject&);
        void slot_applyResult_album_tracks(const QJsonArray&);

        void slot_applyResult_checkRating_album(const QJsonObject&);
        void slot_applyResult_getRating_album(const QJsonArray&);
        void slot_applyResult_addRating_album(const QJsonObject&);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void changedOnlyTabUI_notSendSignal(QString p_step);

        void slot_change_device_state(const QString&);

    private:
        // function about UI
        void setUIControl_basic();

    private:
        Linker *linker;

        apple::AppleSettings *appleSettings;

        AbstractImageDetailContents_RHV *album_detail_info;
        AlbumTrackDetailInfo_RHV *album_tracks[99999];

        // UI
        QVBoxLayout *box_login_contents;
        QWidget *widget_login_contents;

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
        apple::AlbumItemData data_album;

        QJsonObject login_wait_Object;
        QJsonObject jsonObj_albumInfo;
        QJsonArray jsonArr_tracks_toPlay;                       ///< Track 전체를 재생하기 위함

        QString contentStep;

        int roseServer_albumId = 0;
        int roseServer_trackId[99999];

        int track_favoriteOffset = 0;
        int track_id_fav = 0;
        int track_idx_fav = 0;

        int album_star_fav = 0;
        int track_star_fav = 0;

        bool flag_album_fav = false;
        bool flag_track_fav = false;
        bool flag_send_album = false;
        bool flag_check_album = false;
        bool flag_send_track = false;
        bool flag_check_track = false;

        bool flag_track_ok = false;
        bool flag_credit_ok = false;
    };
};
#endif // APPLEALBUMDETAIL_H
