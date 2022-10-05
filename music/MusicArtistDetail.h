#ifndef MUSICARTISTDETAIL_H
#define MUSICARTISTDETAIL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemAlbum_rosehome.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"

#include <QCoreApplication>


namespace music {

    /**
     * @brief Artist의 상세보기 화면
     */
    class ArtistDetail : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit ArtistDetail(QWidget *parent = nullptr);
        ~ArtistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

        // albumImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

    private:
        void setDataTrackFromDB();
        void setDataAlbumFromDB();

        void setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId);

        int get_rose_playType(OptMorePopup::ClickMode clickMode);
        QJsonArray get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode);
        QJsonArray subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex);
        QJsonArray reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex);

        QJsonArray getDataForPlayMusic(const int);

    private:
        Linker *linker;

        AbstractImageDetailContents_RHV *artist_detail_info;
        PlaylistTrackDetailInfo_RHV *artist_tracks[5];
        roseHome::ItemAlbum_rosehome *artist_albums[24];

        QList<roseHome::AlbumItemData> *list_artistAlbums;

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vbox_trackList;
        QVBoxLayout *vbox_albumList;
        QWidget *widget_album;
        FlowLayout *flow_albums;

        QLabel *lb_subTitle[5];
        QPushButton *btnView_all[5];

        QString artist_name;
        QString artist_image;

        QJsonArray jsonArrTracks;

        bool flag_track_ok = false;
    };
};
#endif // MUSICARTISTDETAIL_H
