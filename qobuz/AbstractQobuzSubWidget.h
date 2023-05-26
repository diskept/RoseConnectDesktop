#ifndef ABSTRACTQOBUZSUBWIDGET_H
#define ABSTRACTQOBUZSUBWIDGET_H

#include "common/AbstractCommonSubWidget.h"

#include "delegate/AbstractTidalTrackDelegate.h"

#include "login/dialog_playlist_onRose.h"
#include "login/dialoglogin_forqobuz.h"     // Qobuz Log-in
#include "login/dialoglogin_waitforqobuz.h"

#include "widget/AbstractImageDetailInfo.h"
#include "widget/AbstractImageDetailInfo_RHV.h"
#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogArtistList.h"

#include "qobuz/qobuz_struct.h"
#include "qobuz/Dialog_ChoosePlaylist_forQobuz.h"

#include "bugs/BugsChooseFilterOpt.h"       // for filtering
#include "tidal/AbstractItem.h"

#include <QJsonArray>
#include <QStackedWidget>



#define print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {
    /**
     * @brief Qobuz 서브 페이지를 구성하는데 있어, 상속체계의 부모가 되는 클래스
     * @details 공통으로 사용하는 시그널 및 공통 클래스 구성 스타일을 정의함 (클래스 변수 및 형태)
     */
    class AbstractQobuzSubWidget : public common::AbstractCommonSubWidget
    {
        Q_OBJECT
    public:
        explicit AbstractQobuzSubWidget(MainUIType p_mainUIType, QWidget *parent = nullptr);
        ~AbstractQobuzSubWidget();

    protected slots:

        // ProcCommon 으로부터 데이터 수신 처리 - 데이터 받는 경우 override 처리할 것
        virtual void slot_applyResult_playlists(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_mostPopular(const QJsonObject&, const QString){ }
        virtual void slot_applyResult_artists(const QList<qobuz::ArtistItemData>&, const bool){ }
        virtual void slot_applyResult_videos(const QList<qobuz::VideoItemData>&, const bool){ }

        virtual void slot_applyResult_items_of_playlist(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool){ }

        virtual void slot_applyResult_myCreatedPlaylists(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_myFavoritePlaylists(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_myFavoriteAlbums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_myFavoriteTracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_myFavoriteArtists(const QList<qobuz::ArtistItemData>&, const bool){ }
        virtual void slot_applyResult_myFavoriteVideos(const QList<qobuz::VideoItemData>&, const bool){ }


        // ItemWidget 관련 - Qobuz 전용
        virtual void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode){ }

        // AbstractImageDetailInfo 관련 - 타이달 전용 (Album 상세, Playlist 상세)
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode){ }
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailInfo_forQobuz::BtnClickMode){ }            //Added diskept 07/01/2021
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailInfo_RHV::BtnClickMode){ }                 //Added diskept 09/02/2021
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode){ }             //Added diskept 01/07/2022

        // ListWidget 관련 - Qobuz 전용
        virtual void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode){ }
        virtual void slot_clickedItemAlbum_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode){ }
        virtual void slot_thumbnailDownloaded_fotListWidget_track(){ }


        // Qobuz 로그인 관련
        void slot_clickBtnLogin_toShowDialog();
        virtual void slot_acceptedDialogLogin();
        virtual void slot_acceptedDialogLogout();

        // 로그인 여부
        bool is_qobuz_logined();
        void showNeededLoginQobuz();


    protected:

        QWidget* get_createUIControl_notLoginView();

        // 자식 클래스에서 화면에 UI를 생성하기 위한 함수들
        void createList_itemAlbum_applyingWithData(const QList<qobuz::AlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemPlaylsit_applyingWithData(const QList<qobuz::PlaylistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemArtist_applyingWithData(const QList<qobuz::ArtistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemVideo_applyingWithData(const QList<qobuz::VideoItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemTrackDelegate_applyingWithData(const QList<qobuz::TrackItemData> &list_data, QListWidget *p_listWidget, bool flagAdjustHeight);
        void createList_itemTrackDelegate_applyingWithData_showImage(const QList<qobuz::TrackItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight);
        void createList_itemAlbumDelegate_applyingWithData_showImage(const QList<qobuz::AlbumItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight);

        void applyImageData_itemTrackDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<qobuz::TrackItemData>* p_list_track);
        void applyImageData_itemAlbumDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<qobuz::AlbumItemData>* p_list_album);

        // 자식 클래스에서 Item의 클릭을 처리하기 위한 함수들
        void proc_clicked_itemPlaylist(qobuz::PlaylistItemData& data_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemPlaylist(QList<qobuz::PlaylistItemData>* list_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemAlbum(qobuz::AlbumItemData& data_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemAlbum(QList<qobuz::AlbumItemData>* list_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemArtist(qobuz::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemArtist(QList<qobuz::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemVideo(qobuz::VideoItemData& data_video, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemVideo(QList<qobuz::VideoItemData>* list_video, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(qobuz::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<qobuz::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(qobuz::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const AlbumTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<qobuz::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const AlbumTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(qobuz::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<qobuz::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemAlbum_inList(qobuz::AlbumItemData& data_album, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemAlbum_inList(QList<qobuz::AlbumItemData>* data_album, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemAlbum_inList(qobuz::AlbumItemData& data_album, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemAlbum_inList(QList<qobuz::AlbumItemData>* data_album, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemHistory(roseHome::HistoryItemData& data_history, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemHistory(QList<roseHome::HistoryItemData>* list_history, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);


        // My created playlist 처리에 대해서 UI갱신 처리
        virtual void proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData&){ };



        // 자식 클래스에서 OptMorePopup 클릭에 대한 상세처리하기 위한 함수들
        void proc_clicked_optMorePopup_fromAlbum(qobuz::AlbumItemData& data_album, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromAlbum(QList<qobuz::AlbumItemData>* list_album, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromVideo(qobuz::VideoItemData& data_video, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromVideo(QList<qobuz::VideoItemData>* list_video, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromPlaylist(QList<qobuz::PlaylistItemData>* list_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromTrack(qobuz::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromTrack(QList<qobuz::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromArtist(qobuz::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromArtist(QList<qobuz::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode);


        // 자식 클래스에서 DetailInfo 클릭에 대한 상세처리하기 위한 함수들
        void proc_clicked_imageDetailInfo_fromAlbum(qobuz::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromAlbum(qobuz::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromAlbum(qobuz::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromArtist(qobuz::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromArtist(qobuz::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromArtist(qobuz::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode);
        //void proc_clicked_imageDetailInfo_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_forQobuz::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode);


        // 자식 클래스에서 페이지 이동의 간편화
        void movePage_video_allView(qobuz::PageInfo_VideoAllView& data_pageInfo);
        void movePage_track_allView(qobuz::PageInfo_TrackAllView& data_pageInfo);
        void movePage_album_allView(qobuz::PageInfo_AlbumAllView& data_pageInfo);
        void movePage_sameArtist_allView(qobuz::PageInfo_AlbumAllView& data_pageInfo);
        void movePage_playlist_allView(qobuz::PageInfo_PlaylistAllView& data_pageInfo);
        void movePage_similarPlaylist_allView(qobuz::PageInfo_PlaylistAllView& data_pageInfo);
        void movePage_playlist_editOfMine(qobuz::PlaylistItemData& data_playlist);

        void movePage_artist_detailView(const QList<int>& list_artist_id, const QList<QString>& list_artist_name);
        //void movePage_album_detailView(const int album_id);
        void movePage_album_detailView(const QString album_id);


        // 자식 클래스에서 사용자의 플레이리스트 담기 관련
        void showDialog_toAddMyCollection(QVariant content_id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection content_type);


        // Filtering 관련
        static QList<bugs::FilterOptionData> get_list_filterOptData_forMyCollection();


    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220824_4
        void slot_cannotRosePlay(const QString errorMsg);
        void slot_done_addPlaylist();

        void slot_add_choosePlaylist_forTrack(QString playlist_uuid_target, int track_id);
        void slot_add_choosePlaylist_forAlbum(QString playlist_uuid_target, int album_id);
        void slot_add_choosePlaylist_forPlaylist(QString playlist_uuid_target, QString playlist_uuid);

        void slot_add_rosePlaylist_withQobuz(const int&, const QJsonObject&);

        void slot_dlg_signal_artist(const QJsonObject &p_jsonObject);


    private:
        Linker *linker;

        DialogArtistList *dlgArtist;    // Qobuz Artist list Popup Dialog
        DialogLogin_forQobuz *dlbLogin;

    };
};

#endif // ABSTRACTQOBUZSUBWIDGET_H
