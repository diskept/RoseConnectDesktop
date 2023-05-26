#ifndef QOBUZALBUMDETAIL_H
#define QOBUZALBUMDETAIL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemAlbum_forQobuz.h"
#include "qobuz/ItemArtist_forQobuz.h"
#include "qobuz/QobuzSettings.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"
#include "widget/myqwidget.h"//c220729

#include <QCoreApplication>


namespace qobuz {

#if 0
    /**
     * @brief 버튼 형식의 라벨
     * @note 설정-입출력설정에서 사용
     */
    class BtnLabel : public QPushButton//c220816
    {
        Q_OBJECT
    public:
        BtnLabel(QString p_text, QString p_objName, QString p_imgPath_selected, QString p_imgPath_unSelected, QWidget *parent = nullptr);
        void setStyleSelected(bool p_flagSelected);
        void setStyleText(QString a);
        void setText(QString p_text);
        void setImgPath(QString p_imgPath_selected, QString p_imgPath_unSelected);

    private:
        QLabel *lb_image;
        QLabel *lb_text;
        //QString imgPath_selected;
        //QString imgPath_unSelected;

        QPixmap *pixmap_selected;
        QPixmap *pixmap_unSelected;

    };
#endif

    /**
         * @brief Qobuz의 Album 상세화면 클래스
         */
    class QobuzAlbumDetail : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzAlbumDetail(QWidget *parent = nullptr);
        ~QobuzAlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        // AlbumImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode) override;

        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_changedSubTabUI(const QJsonObject &p_data);
        void changedOnlyTabUI_notSendSignal(QString p_step);

        void slot_applyResult_creditInfo(const int);

        void slot_applyResult_albumInfo(qobuz::AlbumItemData data_album);
        void slot_applyResult_sameArtist(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_suggestArtsit(const QList<qobuz::ArtistItemData>&, const bool);

        void slot_applyResult_checkRating_album(const QJsonObject&);
        void slot_applyResult_checkRating_track(const QJsonObject&);

        void slot_applyResult_addRating_album(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);

        void slot_applyResult_getRating_album(const QJsonArray&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_applyResult_getShareLink(const QString&);             //220817 share link

        void slot_clickBtn_subTitle_viewAll();

        void slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject&);

        void slot_qobuz_logChange();

    private:
        // function about UI
        void setUIControl_basic();
        void setUIControl_sameArtsit();
        void setUIControl_suggestArtsits();

        void setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum();
        QHBoxLayout* setUIControl_hBoxLayout_forArtist();

    private:
        Linker *linker;

        DialogNotice *dlgAlbumInfo;

        qobuz::QobuzSettings *qobuzSettings;
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
        qobuz::ItemAlbum_qobuz *sameArtist[15];
        qobuz::ItemArtist_qobuz *artistSuggestions[15];

        qobuz::AlbumItemData data_album;
        QList<qobuz::TrackItemData> *list_track;            ///< Album 내의 Track 정보
        QList<qobuz::AlbumItemData> *list_sameArtist;
        QList<qobuz::ArtistItemData> *list_suggestArtist;

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

#endif // QOBUZALBUMDETAIL_H
