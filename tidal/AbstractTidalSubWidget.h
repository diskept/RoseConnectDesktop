#ifndef ABSTRACTTIDALSUBWIDGET_H
#define ABSTRACTTIDALSUBWIDGET_H

#include "common/AbstractCommonSubWidget.h"
#include "delegate/AbstractTidalTrackDelegate.h"

#include "login/dialog_playlist_onRose.h"
#include "login/dialoglogin_fortidal.h"     // tidal Log-in

#include "widget/AbstractImageDetailInfo.h"
#include "widget/AbstractImageDetailInfo_RHV.h"
#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogArtistList.h"
#include "widget/dialogNotice.h"

#include "tidal/AbstractItem.h"
#include "tidal/Dialog_ChoosePlaylist_forTidal.h"
#include "tidal/ItemAlbum.h"
#include "tidal/ItemPlaylist.h"
#include "tidal/ItemArtist.h"
#include "tidal/ItemVideo.h"
#include "tidal_struct.h"

#include "bugs/BugsChooseFilterOpt.h"       // for filtering

#include "qobuz/Dialog_ChoosePlaylist_forQobuz.h"   //for filtering... Added Jeon 16/12/2020

#include <QJsonArray>
#include <QStackedWidget>



#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {
    /**
     * @brief 타이달 서브 페이지를 구성하는데 있어, 상속체계의 부모가 되는 클래스
     * @details 공통으로 사용하는 시그널 및 공통 클래스 구성 스타일을 정의함 (클래스 변수 및 형태)
     */
    class AbstractTidalSubWidget : public common::AbstractCommonSubWidget
    {
        Q_OBJECT

    public:
        explicit AbstractTidalSubWidget(MainUIType p_mainUIType, QWidget *parent = nullptr);
        ~AbstractTidalSubWidget();

        virtual void applyLoginSuccessState(){ }            ///< 로그인 성공하면 호출되는 함수임. 자식 클래스에서 필요시 재정의

    protected slots:


        // ProcCommon 으로부터 데이터 수신 처리 - 데이터 받는 경우 override 처리할 것
        virtual void slot_applyResult_topHit(const QList<tidal::ArtistItemData>&, const bool){ }
        virtual void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_artists(const QList<tidal::ArtistItemData>&, const bool){ }
        virtual void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_videos(const QList<tidal::VideoItemData>&, const bool){ }

        virtual void slot_applyResult_items_of_playlist(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool){ }

        virtual void slot_applyResult_myCreatedPlaylists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_myFavoritePlaylists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_myFavoriteAlbums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_myFavoriteTracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_myFavoriteArtists(const QList<tidal::ArtistItemData>&, const bool){ }
        virtual void slot_applyResult_myFavoriteVideos(const QList<tidal::VideoItemData>&, const bool){ }


        // ItemWidget 관련 - 타이달 전용
        virtual void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode){ }

        // AbstractImageDetailInfo 관련 - 타이달 전용 (Album 상세, Playlist 상세)
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode){ }
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode){ }             //Added Jeon 02/17/2022

        // ListWidget 관련 - 타이달 전용
        virtual void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode){ }
        virtual void slot_thumbnailDownloaded_fotListWidget_track(){ }


        // Tidal 로그인 관련
        virtual void slot_acceptedDialogLogin();
        virtual void slot_acceptedDialogLogout();


        void slot_clickBtnLogin_toShowDialog();
        void slot_dialogLogin_acceptedAuthCode();
        void slot_getToken(const bool flagSuccess, const QString errorMsg);
        void slot_completeReq_getSessionInfo(const QString errorMsg, const QJsonObject& jsonObj_session);
        void slot_completeReq_getSubscription(const QString errorMsg, const QJsonObject& jsonObj_subscription);
        void slot_completeReq_get_session_info(const RoseSessionInfo_forTidal& sessionInfo);


        // 로그인 여부
        bool is_tidal_logined();
        void showNeededLoginTidal();


    protected:

        // 초기 UI화면 구성
        void setUIControl_notLogin();
        //virtual void setUIControl_logined();            // 자식 클래스에서, 자신에게 맞는 Widget 만들기

        QWidget* get_createUIControl_notLoginView();

        // 자식 클래스에서 화면에 UI를 생성하기 위한 함수들
        void createList_itemAlbum_applyingWithData(const QList<tidal::AlbumItemData> &list_data, AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemPlaylsit_applyingWithData(const QList<tidal::PlaylistItemData> &list_data, AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemArtist_applyingWithData(const QList<tidal::ArtistItemData> &list_data, AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemVideo_applyingWithData(const QList<tidal::VideoItemData> &list_data, AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemTrackDelegate_applyingWithData(const QList<tidal::TrackItemData> &list_data, QListWidget *p_listWidget, bool flagAdjustHeight);
        void createList_itemTrackDelegate_applyingWithData_showImage(const QList<tidal::TrackItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight);

        void applyImageData_itemTrackDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<tidal::TrackItemData>* p_list_track);

        // 자식 클래스에서 Item의 클릭을 처리하기 위한 함수들
        void proc_clicked_itemPlaylist(tidal::PlaylistItemData& data_playlist, const AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemPlaylist(QList<tidal::PlaylistItemData>* list_playlist, const AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemAlbum(tidal::AlbumItemData& data_album, const AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemAlbum(QList<tidal::AlbumItemData>* list_album, const AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemArtist(tidal::ArtistItemData& data_artist, const AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemArtist(QList<tidal::ArtistItemData>* list_artist, const AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemVideo(tidal::VideoItemData& data_video, const AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemVideo(QList<tidal::VideoItemData>* list_video, const AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(tidal::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<tidal::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(tidal::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const AlbumTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<tidal::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const AlbumTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(tidal::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<tidal::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);




        // My created playlist 처리에 대해서 UI갱신 처리
        virtual void proc_preUpdate_delete_myCreatePlaylist(const ItemPositionData&){ };



        // 자식 클래스에서 OptMorePopup 클릭에 대한 상세처리하기 위한 함수들
        void proc_clicked_optMorePopup_fromAlbum(tidal::AlbumItemData& data_album, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromAlbum(QList<tidal::AlbumItemData>* list_album, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromVideo(tidal::VideoItemData& data_video, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromVideo(QList<tidal::VideoItemData>* list_video, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromPlaylist(tidal::PlaylistItemData& data_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromPlaylist(QList<tidal::PlaylistItemData>* list_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromTrack(tidal::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromTrack(QList<tidal::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromArtist(tidal::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromArtist(QList<tidal::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode);


        // 자식 클래스에서 DetailInfo 클릭에 대한 상세처리하기 위한 함수들
        void proc_clicked_imageDetailInfo_fromAlbum(tidal::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromAlbum(tidal::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromArtist(tidal::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromArtist(tidal::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromPlaylist(tidal::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromPlaylist(tidal::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode);


        // 자식 클래스에서 페이지 이동의 간편화
        void movePage_video_allView(tidal::PageInfo_VideoAllView& data_pageInfo);
        void movePage_track_allView(tidal::PageInfo_TrackAllView& data_pageInfo);
        void movePage_album_allView(tidal::PageInfo_AlbumAllView& data_pageInfo);
        void movePage_playlist_allView(tidal::PageInfo_PlaylistAllView& data_pageInfo);
        void movePage_playlist_editOfMine(tidal::PlaylistItemData& data_playlist);

        void movePage_artist_detailView(const QList<int>& list_artist_id, const QList<QString>& list_artist_name);
        void movePage_album_detailView(const int album_id);


        // 자식 클래스에서 사용자의 플레이리스트 담기 관련
        void showDialog_toAddMyCollection(QVariant content_id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection content_type);


        // Filtering 관련
        static QList<bugs::FilterOptionData> get_list_filterOptData_forMyCollection();


    private slots:

        //void slot_dialogLogin_acceptedAuthCode();
        //void slot_getToken(const bool flagSuccess, const QString errorMsg);
        //void slot_completeReq_getMemberInfo(const QString errorMsg, const QJsonObject& jsonObj_member, const QJsonObject& jsonObj_product);

        void slot_cannotRosePlay(const QString errorMsg);
        void slot_done_addPlaylist();

        void slot_add_choosePlaylist_forTrack(QString playlist_uuid_target, int track_id);
        void slot_add_choosePlaylist_forAlbum(QString playlist_uuid_target, int album_id);
        void slot_add_choosePlaylist_forPlaylist(QString playlist_uuid_target, QString playlist_uuid);

        void slot_add_rosePlaylist_withTidal(const int&, const QJsonObject&);

        void slot_dlg_signal_artist(const QJsonObject &p_jsonObject);

        void slot_applyResult_getShareLink(const QString&);//c220826_2


    private:

        Linker *linker;

        DialogArtistList *dlgArtist;    // Tidal Artist list Popup Dialog

    };
};

#endif // ABSTRACTTIDALSUBWIDGET_H
