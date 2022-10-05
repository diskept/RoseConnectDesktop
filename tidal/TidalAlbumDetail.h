#ifndef TIDALALBUMDETAIL_H
#define TIDALALBUMDETAIL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"
#include "tidal/ItemArtist.h"
#include "tidal/TidalSettings.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal의 Album 상세화면 클래스
     */
    class TidalAlbumDetail : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalAlbumDetail(QWidget *parent = nullptr);
        ~TidalAlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    public slots:
        void slot_changedSubTabUI(const QJsonObject &p_data);

    protected slots:
        // AlbumImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;

        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void changedOnlyTabUI_notSendSignal(QString p_step);

        //void slot_applyResult_creditInfo(const int);

        void slot_applyResult_albumInfo(tidal::AlbumItemData data_album);
        void slot_applyResult_sameArtist(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_suggestArtsit(const QList<tidal::ArtistItemData>&, const bool);

        void slot_applyResult_checkRating_album(const QJsonObject&);
        void slot_applyResult_checkRating_track(const QJsonObject&);

        void slot_applyResult_addRating_album(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_applyResult_getRating_album(const QJsonArray&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_clickBtn_subTitle_viewAll();

        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_tidal_logChange();

    private:
        // function about UI
        void setUIControl_basic();
        void setUIControl_sameArtsit();
        void setUIControl_suggestArtsits();

        void setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum();
        QHBoxLayout* setUIControl_hBoxLayout_forArtist();

    private:
        void slot_applyResult_getShareLink(const QString &link);//c220818
        Linker *linker;
        tidal::TidalSettings *tidalSettings;

        DialogNotice *dlgAlbumInfo;

        AbstractImageDetailContents_RHV *album_detail_info;
        AlbumTrackDetailInfo_RHV *album_tracks[9999];
        AlbumTrackDetailInfoCredit_RHV *album_credit[9999];

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

        QHBoxLayout *hBox_sameArtist;
        QHBoxLayout *hBox_suggestArtist;

        // data
        tidal::ItemAlbum *sameArtist[15];
        tidal::ItemArtist *artistSuggestions[15];

        tidal::AlbumItemData data_album;
        QList<tidal::TrackItemData> *list_track;            ///< Album 내의 Track 정보
        QList<tidal::AlbumItemData> *list_sameArtist;
        QList<tidal::ArtistItemData> *list_suggestArtist;

        QJsonObject login_wait_Object;
        QJsonArray jsonArr_tracks_toPlay;                  ///< Track 전체를 재생하기 위함

        QString contentStep;

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

#endif // TIDALALBUMDETAIL_H
