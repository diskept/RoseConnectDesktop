#ifndef ROSEHOMEFAVORITE_H
#define ROSEHOMEFAVORITE_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"

#include "tidal/PushBtn_withID.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QCoreApplication>


namespace roseHome {
    /**
     * @brief "RoseHome > 홈" 화면
     */
    class RoseFavorite : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:

        explicit RoseFavorite(QWidget *parent = nullptr);
        ~RoseFavorite();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_changedSubTabUI();
        void slot_changedSubTabUI(const QJsonObject &p_data) override;

        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        // about clicks
        void slot_clickBtn_Filter();
        void slot_clickBtn_Filter_close();

        void slot_clickBtn_chooseDetailMedia();

        void slot_applyResult_favoriteTracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_favoriteAlbums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_favoritePlaylists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getRating_track(const QJsonArray&);

    private:
        // 초기 UI화면 구성
        void setUIControl_chooseFilter();
        void setUIControl_chooseFilter_fromData();
        void setUIControl_chooseStep(QString type);

        void request_more_Data(QString type);

    private:
        Linker *linker;

        QString page = "";

        QString btnStyle_normal;
        QString btnStyle_selected;

        // 관리 필요한 Layout UI
        //---------------------------------------------------------------------------------------  Filter : START
        QHBoxLayout *box_media_filter;
        FlowLayout *flow_media_filter;
        FlowLayout *flowLayout_media;           ///< Meida part
        FlowLayout *flowLayout_order;           ///< Meida part

        QWidget *widget_filter;                 ///< Meida filter
        QPushButton *btn_filter_ico;            ///< Meida filter icon
        QPushButton *btn_filter[30];            ///< Meida button
        QPushButton *btn_order[10];             ///< Meida button

        QLabel *lb_media;
        QLabel *lb_order;

        bool flag_filter[30];
        bool flagSet_media = false;             ///< Media 카테고리 데이터 세팅 여부

        int btn_filter_cnt = 0;
        int enable_media_id[30];

        QString str_media_name[30];

        bool flag_order[10];
        bool flagSet_order = false;

        int btn_order_cnt = 0;
        int enable_order_id[30];

        QString str_order_name[10];

        QStringList mediaTypeList;
        QStringList orderTypeList;
        //---------------------------------------------------------------------------------------  Filter : END

        //---------------------------------------------------------------------------------------  STEP : START
        QPushButton *btn_step_track;
        QPushButton *btn_step_album;
        QPushButton *btn_step_playlist;

        QString contentStep = "";

        QVBoxLayout *box_main_contents;              ///< Rosehome Favorite Body Layout

        QStackedWidget *stackedWidget_body;

        QWidget *widget_tab_track;
        QWidget *widget_tab_album;
        QWidget *widget_tab_playlist;

        QVBoxLayout *box_favorite_track;
        FlowLayout *flowLayout_favorite_album;
        FlowLayout *flowLayout_favorite_playlist;

        bool step_change = false;
        bool filter_change = false;
        //---------------------------------------------------------------------------------------  STEP : END

        PlaylistTrackDetailInfo_RHV *favorite_track[999999];
        roseHome::ItemAlbum_rosehome *favorite_album[99999];
        roseHome::ItemPlaylist_rosehome *favorite_playlist[99999];

        // data
        QList<roseHome::TrackItemData> *list_favoriteTrack;
        QList<roseHome::AlbumItemData> *list_favoriteAlbum;
        QList<roseHome::PlaylistItemData> *list_favoritePlaylist;

        QJsonArray jsonArr_tracks_toPlay;                       ///< Track 전체를 재생하기 위함

        bool flagReqMore_track = false;
        bool flag_lastPage_track = false;
        bool flag_draw_track = false;

        int track_next_offset = 0;
        int track_totalCount = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;
        bool flag_draw_album = false;

        int album_next_offset = 0;
        int album_totalCount = 0;

        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;
        bool flag_draw_playlist = false;

        int playlist_next_offset = 0;
        int playlist_totalCount = 0;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;

        bool flag_track_fav = false;
        bool flag_send_track = false;
        bool flag_check_track = false;

    };
};
#endif // ROSEHOMEFAVORITE_H
