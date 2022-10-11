#ifndef BUGSALBUMDETAIL_H
#define BUGSALBUMDETAIL_H

#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ItemAlbum_bugs.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#include <QCoreApplication>


namespace bugs {

    /**
     * @brief 벅스 일반앨범 상세 페이지에 대한 클래스
     */
    class BugsAlbumDetail : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsAlbumDetail(QWidget *parent = nullptr);
        ~BugsAlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:

        // ArtistImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<bugs::AlbumItemData>&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        // function about UI
        void setUIControl_basic();
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);

    private slots:
        void slot_changedSubTabUI(const QJsonObject &p_data);
        void changedOnlyTabUI_notSendSignal(QString p_step);

        void slot_applyResult_albumInfo(bugs::AlbumItemData data_album);
        void slot_applyResult_getShareLink(const QString &link);//c220818

        void slot_applyResult_checkRating_album(const QJsonObject&);
        void slot_applyResult_getRating_album(const QJsonArray&);
        void slot_applyResult_addRating_album(const QJsonObject&);

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_clickBtn_subTitle_viewAll();

    private:
        AbstractImageDetailContents_RHV *album_detail_info;
        AlbumTrackDetailInfo_RHV *album_tracks[9999];

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

        QWidget *widget_artistAlbum;

        QVBoxLayout *vBox_artistAlbum;
        QHBoxLayout *hBox_artistAlbum;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        bugs::ItemAlbum_bugs *artistAlbum[15];

        //data
        bugs::AlbumItemData data_album;

        QList<bugs::TrackItemData> *list_track;
        QList<bugs::AlbumItemData> *list_aritst_album;

        QJsonArray jsonArr_tracks_toPlay;           ///< Track 전체를 재생하기 위함 (list_track 관련)

        QString contentStep;

        int album_track_count = 0;

        int roseServer_albumId = 0;
        int roseServer_trackId[99999];

        int track_favoriteOffset = 0;
        int track_id_fav = 0;
        int track_idx_fav = 0;

        int album_star_fav = 0;
        int track_star_fav = 0;

        bool flag_album_fav = false;
        bool flag_send_album = false;
        bool flag_check_album = false;

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        bool flag_track_ok = false;
        bool flag_credit_ok = false;
    };

};


#endif // BUGSALBUMDETAIL_H
