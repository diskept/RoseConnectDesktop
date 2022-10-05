#ifndef TIDALMYCOLLECTION_H
#define TIDALMYCOLLECTION_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"
#include "tidal/ItemArtist.h"
#include "tidal/ItemPlaylist.h"

#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"

#include <QStackedWidget>
#include <QWidget>
#include "AbstractTidalSubWidget.h"
#include "delegate/TidalTrackDelegate.h"


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

        void setActivePage() override;

    protected slots:
        void slot_acceptedDialogLogin() override;

        void slot_clickBtn_subTitle_side(const int) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_thumbnailDownloaded_fotListWidget_track() override;

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
        void proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData&) override;

    private:
        // 초기 UI화면 구성
        void setUIControl_notLogin();
        void setUIControl_logined();

        // getting data
        void request_playlistData_tidal();


        QWidget *widget_logined = nullptr;
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

        QJsonArray jsonArr_tracks_toPlay;      ///< for 바로 재생을 위해

    };

}

#endif // TIDALMYCOLLECTION_H
