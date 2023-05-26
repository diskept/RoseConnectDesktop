#ifndef TIDALMYCOLLECTION_H
#define TIDALMYCOLLECTION_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"
#include "tidal/ItemArtist.h"
#include "tidal/ItemPlaylist.h"
#include "tidal/TidalSettings.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
//#include "delegate/TidalTrackDelegate.h"

#include <QStackedWidget>
#include <QWidget>

#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal > My Collection 화면 클래스
     */
    class TidalMycollection : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalMycollection(QWidget *parent = nullptr);
        ~TidalMycollection();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        //void slot_acceptedDialogLogin() override;
        //void slot_clickBtn_subTitle_side(const int) override;

        // about Item
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode) override;
        //void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        //void slot_thumbnailDownloaded_fotListWidget_track() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

        void slot_applyResult_myCreatedPlaylists(const QList<tidal::PlaylistItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage)  override;
        void slot_applyResult_myFavoritePlaylists(const QList<tidal::PlaylistItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage)  override;
        void slot_applyResult_myFavoriteAlbums(const QList<tidal::AlbumItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage) override;
        void slot_applyResult_myFavoriteTracks(const QList<tidal::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage) override;
        void slot_applyResult_myFavoriteArtists(const QList<tidal::ArtistItemData>& list_data, const bool flag_lastPage) override;
        void slot_applyResult_myFavoriteVideos(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage) override;

    protected:
        // My created playlist 처리에 대해서 UI갱신 처리
        //void proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData&) override;

    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&);

    private:
        // 초기 UI화면 구성
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forArtist(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forVideo(QLayout *p_layout);

        // 초기 UI화면 구성
        //void setUIControl_notLogin();
        //void setUIControl_logined();

        // getting data
        //void request_playlistData_tidal();

    private:
        Linker *linker;//cheon211008

        QString page = "";

        // 관리 필요한 Layout UI
        tidal::TidalSettings *tidalSettings;

        PlaylistTrackDetailInfo_RHV *collection_track[5];

        tidal::ItemPlaylist *collection_playlist_tidal[15];
        tidal::ItemPlaylist *collection_playlist_create[15];
        tidal::ItemAlbum *collection_album[15];
        tidal::ItemArtist *collection_artist[15];
        tidal::ItemVideo *collection_video[15];

        // UI
        QLabel *label_mainTitle;

        QLabel *lb_subTitle[15];
        QPushButton *btnView_all[15];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_playlistCreate;
        QWidget *widget_playlistTIDAL;
        QWidget *widget_album;
        QWidget *widget_track;
        QWidget *widget_artist;
        QWidget *widget_video;

        QVBoxLayout *vBox_playlistCreate;
        QVBoxLayout *vBox_playlistTIDAL;
        QVBoxLayout *vBox_album;
        QVBoxLayout *vBox_track;
        QVBoxLayout *vBox_artist;
        QVBoxLayout *vBox_video;

        QHBoxLayout *hBox_playlistCreate;
        QHBoxLayout *hBox_playlistTIDAL;
        QHBoxLayout *hBox_album;
        QHBoxLayout *hBox_artist;
        QHBoxLayout *hBox_video;

        QList<tidal::PlaylistItemData> *list_playlist_created;
        QList<tidal::PlaylistItemData> *list_playlist_tidal;
        QList<tidal::AlbumItemData> *list_album;
        QList<tidal::TrackItemData> *list_track;
        QList<tidal::ArtistItemData> *list_artist;
        QList<tidal::VideoItemData> *list_video;

        QJsonArray jsonArr_tracks_toPlay;

        bool flag_playlist_created[2] = {false, false};
        bool flag_playlist_tidal[2] = {false, false};
        bool flag_album[2] = {false, false};
        bool flag_track[2] = {false, false};
        bool flag_artist[2] = {false, false};
        bool flag_video[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;

        /*QWidget *widget_logined = nullptr;
        QWidget *widget_notLogin = nullptr;

        QLabel *label_mainTile;
        QHBoxLayout *hBox_playlist;         ///< Playlist
        QHBoxLayout *hBox_playlist_tidal;
        QHBoxLayout *hBox_album;            ///< Album
        QHBoxLayout *hBox_artist;           ///< Artist
        QHBoxLayout *hBox_video;            ///< Video

        TidalTrackDelegate *delegate;
        QListWidget *listWidget_track;


        // Data
        QList<tidal::PlaylistItemData> *list_playlist_created;
        QList<tidal::PlaylistItemData> *list_playlist_tidal;
        QList<tidal::AlbumItemData> *list_album;
        QList<tidal::TrackItemData> *list_track;
        QList<tidal::ArtistItemData> *list_artist;
        QList<tidal::VideoItemData> *list_video;

        QJsonArray jsonArr_tracks_toPlay;      ///< for 바로 재생을 위해*/

    };

}

#endif // TIDALMYCOLLECTION_H
