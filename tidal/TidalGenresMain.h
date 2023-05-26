#ifndef TIDALGENRESMAIN_H
#define TIDALGENRESMAIN_H

#include "tidal_struct.h"
#include "tidal/ItemAlbum.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include "AbstractTidalSubWidget.h"


#include <QJsonArray>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {


    /**
     * @brief Tidal > 탐색 > Genres > 장르선택 (장르 하나 선택했을 때의 메인 화면) 클래스
     */
    class TidalGenresMain : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalGenresMain(QWidget *parent = nullptr);
        ~TidalGenresMain();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_videos(const QList<tidal::VideoItemData>&, const bool) override;

    private slots:
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private :
        // 초기 UI화면 구성
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forVideo(QLayout *p_layout);

    private :

        // Data
        QString curr_genre_path;        ///< 현재 선택된 genre path
        QString genre_api_track;        ///< 데이터 요청을 위한 장르 track path (Tidal API 요청 시 필요)
        QString genre_api_album;        ///< 데이터 요청을 위한 장르 album path (Tidal API 요청 시 필요)
        QString genre_api_playlist;     ///< 데이터 요청을 위한 장르 playlist path (Tidal API 요청 시 필요)
        QString genre_api_video;

        PlaylistTrackDetailInfo_RHV *genre_track[5];
        tidal::ItemAlbum *genre_album[10];
        tidal::ItemPlaylist *genre_playlist[10];
        tidal::ItemVideo *genre_video[10];

        // UI
        QLabel *label_mainTitle;        ///< main titel for specific genre's name
        QString genre_name;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_album;
        QWidget *widget_track;
        QWidget *widget_playlist;
        QWidget *widget_video;

        QVBoxLayout *vBox_album;
        QVBoxLayout *vBox_track;
        QVBoxLayout *vBox_playlist;
        QVBoxLayout *vBox_video;

        QHBoxLayout *hBox_album;
        QHBoxLayout *hBox_track;
        QHBoxLayout *hBox_playlist;
        QHBoxLayout *hBox_video;

        QList<tidal::AlbumItemData> *list_album;
        QList<tidal::TrackItemData> *list_track;
        QList<tidal::PlaylistItemData> *list_playlist;
        QList<tidal::VideoItemData> *list_video;

        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        bool flag_album[2] = {false, false};
        bool flag_track[2] = {false, false};
        bool flag_playlist[2] = {false, false};
        bool flag_video[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;

    };

};

#endif // TIDALGENRESMAIN_H
