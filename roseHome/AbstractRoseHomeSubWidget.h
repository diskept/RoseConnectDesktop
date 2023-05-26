#ifndef AbstractRoseHomeSubWidget_H
#define AbstractRoseHomeSubWidget_H

#include "roseHome/rosehome_struct.h"

#include "common/AbstractCommonSubWidget.h"

#include "widget/AbstractImageDetailInfo_RHV.h"
#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogArtistList.h"

#include "bugs/BugsChooseFilterOpt.h"

#include "tidal/AbstractItem.h"

#include <QJsonArray>
#include <QStackedWidget>


namespace roseHome {
    class AbstractRoseHomeSubWidget : public common::AbstractCommonSubWidget
    {
        Q_OBJECT
    public:
        explicit AbstractRoseHomeSubWidget(MainUIType p_mainUIType, QWidget *parent = nullptr);
        ~AbstractRoseHomeSubWidget();

    protected slots:

        // ProcCommon 으로부터 데이터 수신 처리 - 데이터 받는 경우 override 처리할 것
        virtual void slot_applyResult_MYplaylists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_NEWplaylists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_POPplaylists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_playlists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_albums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool){ }
        virtual void slot_applyResult_artists(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool){ }

        // ItemWidget 관련 - 로즈홈 전용
        virtual void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode){ }
        virtual void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode){ }

        // AbstractImageDetailInfo 관련 - 로즈홈 전용 (Album 상세, Playlist 상세)
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailInfo_RHV::BtnClickMode){ }
        virtual void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode){ }


        // ListWidget 관련 - 로즈홈 전용/
        virtual void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const AlbumTrackDetailInfo_RHV::ClickMode){ }
        virtual void slot_clickedItemTrack_inList(const int, const PlaylistTrackDetailInfo_RHV::ClickMode){ }
        virtual void slot_clickedItemAlbum_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode){ }
        virtual void slot_thumbnailDownloaded_fotListWidget_track(){ }

    protected:

        // 자식 클래스에서 화면에 UI를 생성하기 위한 함수들
        void createList_itemRecommend_applyingWithData(const QList<roseHome::RecommendsItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemAlbum_applyingWithData(const QList<roseHome::AlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);
        void createList_itemPlaylsit_applyingWithData(const QList<roseHome::PlaylistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section);

        // 자식 클래스에서 Item의 클릭을 처리하기 위한 함수들
        void proc_clicked_itemAlbum(roseHome::AlbumItemData& data_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemAlbum(QList<roseHome::AlbumItemData>* list_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemHistory(roseHome::HistoryItemData& data_history, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemHistory(QList<roseHome::HistoryItemData>* list_history, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemArtist(roseHome::ArtistItemData& data_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemArtist(QList<roseHome::ArtistItemData>* list_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemPlaylist(roseHome::PlaylistItemData& data_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemPlaylist(QList<roseHome::PlaylistItemData>* list_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemPlaylist_rose(roseHome::PlaylistItemData& data_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemPlaylist_rose(QList<roseHome::PlaylistItemData>* list_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section);

        void proc_clicked_itemTrack_inList(roseHome::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);
        void proc_clicked_itemTrack_inList(QList<roseHome::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section);

        // 자식 클래스에서 OptMorePopup 클릭에 대한 상세처리하기 위한 함수들
        void proc_clicked_optMorePopup_fromAlbum(roseHome::AlbumItemData& data_playlist, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromAlbum(QList<roseHome::AlbumItemData>* list_playlist, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromPlaylist(roseHome::PlaylistItemData& data_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromPlaylist(QList<roseHome::PlaylistItemData>* list_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromTrack(roseHome::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromTrack(QList<roseHome::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode);

        void proc_clicked_optMorePopup_fromArtist(roseHome::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode);
        void proc_clicked_optMorePopup_fromArtist(QList<roseHome::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode);


        // 자식 클래스에서 DetailInfo 클릭에 대한 상세처리하기 위한 함수들
        void proc_clicked_imageDetailInfo_fromAlbum(roseHome::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode);
        void proc_clicked_imageDetailInfo_fromPlaylist(roseHome::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode);

        // 자식 클래스에서 페이지 이동의 간편화
        void movePage_album_allView(roseHome::PageInfo_AlbumAllView&);
        void movePage_playlist_allView(roseHome::PageInfo_PlaylistAllView&);
        void movePage_rosetube_allView(roseHome::PageInfo_PlaylistAllView&);
        void movePage_track_allView(roseHome::PageInfo_TrackAllView&);
        void movePage_artist_allView(roseHome::PageInfo_ArtistAllView&);
        void movePage_history_allView(roseHome::PageInfo_HistoryAllView&);

        void movePage_userPage(roseHome::PageInfo_UserPage&);

        void movePage_radio_allView(roseHome::PageInfo_RadioAllView&);

        // Filtering 관련
        static QList<bugs::FilterOptionData> get_list_filterOptData_forMyCollection();

    private slots:
        void slot_applyResult_visualTracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_getShareLink(const QString &link);//c220825_2

        void slot_add_rosePlaylist_withRosetube(const int&, const QJsonObject&);


    //cheon211018---start
    signals:
        /// 부모에게 페이지가 변경되었다고 히스토리 관리하라고 알려줘야한다. (Main에서 이전/다음 버튼 컨트롤하기 위함)
        void clickedSubTab(const QJsonObject &p_data);

    protected slots:
        /// 내부 페이지 변경되었을때 슬롯 함수
        virtual void slot_changedSubTabUI(const QJsonObject &p_data);

    protected:
        /// AbstractMainContent가 보여야할 위젯의 코드
        QString pageCode = "";
        /// 현재 클래스에서 step관리 하는 경우 코드값 (없으면 step이 존재하지 않는다는 뜻임)
        QString contentStep = "";
        virtual void setDataABSPage(QJsonObject p_data);
        virtual QJsonObject getDataJson();

        //cheon211018---end
    };
};

#endif // AbstractRoseHomeSubWidget_H
