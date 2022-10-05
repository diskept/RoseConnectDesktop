#ifndef ABSTRACTBUGSSUBWIDGET_H
#define ABSTRACTBUGSSUBWIDGET_H

#include "common/AbstractCommonSubWidget.h"
#include "delegate/AbstractTidalTrackDelegate.h"

#include "login/dialog_playlist_onRose.h"
#include "login/DialogLogin_forBugs.h"

#include "bugs/bugs_struct.h"
#include "bugs/BugsChooseFilterOpt.h"       // for filtering

#include "tidal/AbstractItem.h"

#include "widget/AbstractImageDetailInfo.h"
#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogArtistList.h"
#include "widget/dialogNotice.h"
#include "widget/dialogVideoList.h"

#include <QJsonArray>
#include <QStackedWidget>


namespace bugs {

    /**
     * @brief Bugs 서브 페이지를 구성하는데 있어, 상속체계의 부모가 되는 클래스
     * 공통으로 사용하는 시그널 및 공통 클래스 구성 스타일을 정의함 (클래스 변수 및 형태)
     */
    class AbstractBugsSubWidget : public common::AbstractCommonSubWidget
    {
        Q_OBJECT
    public:
        explicit AbstractBugsSubWidget(MainUIType p_mainUIType, QWidget *parent = nullptr);
        ~AbstractBugsSubWidget();

        void setActivePage() override;
        virtual void applyLoginSuccessState(){ }            ///< 로그인 성공하면 호출되는 함수임. 자식 클래스에서 필요시 재정의

    protected slots:
        // Bugs 로그인, 로그아웃 관련
        void slot_clickBtnLogin_toShowDialog();
        void slot_clickBtnLogout();

        // 전체재생, 셔플재생 Slot - 필요한 곳에서 overriding
        virtual void slot_btnClicked_playAll(){ }
        virtual void slot_btnClicked_playShuffle(){ }


        // ProcCommon 으로부터 데이터 수신 처리 - 데이터 받는 경우 override 처리할 것
        virtual void slot_applyResult_tracks(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_tracks2(const QList<bugs::TrackItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_albums(const QList<bugs::AlbumItemData>&, const bool){ }
        virtual void slot_applyResult_artists(const QList<bugs::ArtistItemData>&, const bool){ }
        virtual void slot_applyResult_videos(const QList<bugs::VideoItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>&, const bool){ }
        virtual void slot_applyResult_my_albums(const QList<bugs::MyAlbumItemData>&, const bool){ }


        // ItemWidget 관련 - Bugs 전용
        virtual void slot_clickedItemTrackSqure(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemMyAlbum(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode){ }

        virtual void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode){ }

        // AbstractImageDetailInfo 관련 - 타이달 전용 (Album 상세, Playlist 상세)
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode){ }
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode){ }

        // ListWidget 관련 - Bugs 전용
        virtual void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode){ }

        virtual void slot_thumbnailDownloaded_fotListWidget_delegate(){ }


        // Favorite 처리결과에 대한 응답 - Bugs 전용
        virtual void slot_completedReg_favorite_track(const QJsonObject&){ }


    protected:

        bool m_flagCurrLogined;         ///< 현 페이지의 로그인 여부


        // 로그인 여부
        bool is_bugs_logined();
        void showNeededLoginBugs();

        // 초기 UI화면 구성
        void setUIControl_notLogin();
        virtual void setUIControl_logined();            // 자식 클래스에서, 자신에게 맞는 Widget 만들기

        // 전체재생 관련 Widget - 자주 보여서
        QWidget* get_UIControl_btnPlays(const bool flagShowPlay, const bool flagShowShuffle);


        // 자식 클래스에서 화면에 UI를 생성하기 위한 함수들
        void createList_itemTrackSqure_applyingWithData(const QList<bugs::TrackItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int section);
        void createList_itemAlbum_applyingWithData(const QList<bugs::AlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemPD_Album_applyingWithData(const QList<bugs::PD_AlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemMyAlbum_applyingWithData(const QList<bugs::MyAlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
//        void createList_itemPlaylsit_applyingWithData(const QList<bugs::PlaylistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int section);
        void createList_itemArtist_applyingWithData(const QList<bugs::ArtistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemVideo_applyingWithData(const QList<bugs::VideoItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);

        void createList_itemTrackDelegate_applyingWithData(const QList<bugs::TrackItemData> &list_data, QListWidget *p_listWidget, bool flagAdjustHeight);
        void createList_itemTrackDelegate_applyingWithData_showImage(const QList<bugs::TrackItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight);
        void applyImageData_itemTrackDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<bugs::TrackItemData>* p_list_track);

        void createList_itemArtistDelegate_applyingWithData_showImage(const QList<bugs::ArtistItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight);
        void applyImageData_itemArtistDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<bugs::ArtistItemData>* p_list_artist);




        // 자식 클래스에서 Track목록 바로 재생
        void proc_play_allTrack(const QJsonArray& jsonArr_trackToPlay, const bool flagShuffle);
        void proc_play_allVideo(const QJsonArray& jsonArr_videoToPlay, const bool flagShuffle);



        // 자식 클래스에서 Item의 클릭을 처리하기 위한 함수들
        void proc_clicked_itemAlbum(bugs::AlbumItemData& data_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemAlbum(QList<bugs::AlbumItemData>* list_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemPD_Album(bugs::PD_AlbumItemData& data_pd_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemPD_Album(QList<bugs::PD_AlbumItemData>* list_pd_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemMyAlbum(bugs::MyAlbumItemData& data_my_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemMyAlbum(QList<bugs::MyAlbumItemData>* list_my_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);



        void proc_clicked_itemArtist(bugs::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemArtist(QList<bugs::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemArtist_inList(bugs::ArtistItemData& data_artist, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemArtist_inList(QList<bugs::ArtistItemData>* list_artist, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemVideo(bugs::VideoItemData& data_video, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemVideo(QList<bugs::VideoItemData>* list_video, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrackSquare(bugs::TrackItemData& data_album, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrackSquare(QList<bugs::TrackItemData>* list_album, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(bugs::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<bugs::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(bugs::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<bugs::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);


        // Fav 클릭에 대한 처리. 자식 클래스에서 처리해야한다. (list 데이터를 소유하고 있으므로)
        virtual void proc_favoriteClicked_itemTrack_inList(const int, const int){ };


        // Favorite 처리에 대해서 UI갱신 처리
        virtual void proc_preUpdate_favorite_track(const ItemPositionData&){ };
        virtual void proc_preUpdate_favorite_album(const ItemPositionData&){ };
        virtual void proc_preUpdate_favorite_pd_album(const ItemPositionData&){ };
        virtual void proc_preUpdate_favorite_artist(const ItemPositionData&){ };



        // 자식 클래스에서 OptMorePopup 클릭에 대한 상세처리하기 위한 함수들
        void proc_clicked_optMorePopup_fromAlbum(bugs::AlbumItemData& data_album, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromAlbum(QList<bugs::AlbumItemData>* list_album, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromPD_Album(bugs::PD_AlbumItemData& data_pd_album, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromPD_Album(QList<bugs::PD_AlbumItemData>* list_pd_album, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromMyAlbum(bugs::MyAlbumItemData& data_my_album, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromMyAlbum(QList<bugs::MyAlbumItemData>* list_my_album, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromVideo(bugs::VideoItemData& data_video, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromVideo(QList<bugs::VideoItemData>* list_video, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromTrack(bugs::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromTrack(QList<bugs::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromArtist(bugs::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromArtist(QList<bugs::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode);


        // 자식 클래스에서 DetailInfo 클릭에 대한 상세처리하기 위한 함수들
        void proc_clicked_imageDetailInfo_fromAlbum(bugs::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromPDAlbum(bugs::PD_AlbumItemData& data_pd_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromArtist(bugs::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode);

        void proc_clicked_imageDetailInfo_fromArtist(bugs::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode);

        // 자식 클래스에서 페이지 이동의 간편화
        void movePage_track_allView(bugs::PageInfo_TrackAllView& data_pageInfo);
        void movePage_pd_album_allView(bugs::PageInfo_PDAlbumAllView& data_pageInfo);

        void movePage_artist_detailView(const QList<int>& list_artist_id, const QList<QString>& list_artist_name);
        void movePage_artist_detailView(const int artist_id);
        void movePage_album_detailView(const int album_id);
        void movePage_pd_album_detailView(const int es_album_id);
        void movePage_my_album_detailView(bugs::MyAlbumItemData& data_my_album);

        void movePage_video_allView_ofTrack(bugs::TrackItemData& data_track);

        // Filtering 관련
        static QList<bugs::FilterOptionData> get_list_filterOptData_forMyCollection();


    private slots:
        void slot_applyResult_getShareLink(const QString &link);//c220825_1
        void slot_dialogLogin_acceptedAuthCode();
        void slot_getToken(const bool flagSuccess, const QString errorMsg);
        void slot_completeReq_getMemberInfo(const QString errorMsg, const QJsonObject& jsonObj_member, const QJsonObject& jsonObj_product);
        void slot_dlg_signal_video(const QJsonObject &p_jsonObject);
        void slot_dlg_signal_artist(const QJsonObject &p_jsonObject);


    private:
        Linker *linker;

        DialogVideoList *dlgVideo;      // Bugs Music Video list Popup Dialog
        DialogArtistList *dlgArtist;    // Bugs Artist list Popup Dialog

    };

};

#endif // ABSTRACTBUGSSUBWIDGET_H
