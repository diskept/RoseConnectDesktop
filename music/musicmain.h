#ifndef MUSICMAIN_H
#define MUSICMAIN_H

#include "music/musichome.h"
#include "music/musiccategory.h"
#include "music/musicalbum.h"
#include "music/musicfolder.h"
#include "music/musiccollection.h"
#include "music/musicsearch.h"

#include "music/albumdetail.h"
#include "music/artistinfo.h"
#include "music/composerinfo.h"
#include "music/genreinfo.h"
#include "music/metainfo.h"

#include "music/musice_playlist.h"
#include "music/musicfolder_editfavor.h"
#include "music/musiclist_group.h"
#include "music/musiclist_collectiontrack.h"

#include "music/musiclist_artistTrack.h"
#include "music/musiclist_artistAlbum.h"
//#include "music/musiclist_genreTrack.h"
#include "music/musiclist_genreAlbum.h"

#include "music/MusicAlbumDetail.h"
#include "music/MusicAlbumDetail_rose.h"
#include "music/MusicAlbumAll.h"
#include "music/MusicPlaylistDetail_rose.h"
#include "music/MusicPlaylistAll.h"
#include "music/MusicTrackListAll_rose.h"
#include "music/MusicArtistDetail.h"
#include "music/MusicArtistAll.h"
#include "music/MusicMyCollection.h"
#include "music/MusicMyFavorite.h"
#include "music/MusicSearchMain.h"

#include "home/abstractmaincontent.h"
#include "home/topmenubar.h"
#include "home/topnavibar.h"

#include "widget/optionpopup.h"
#include "widget/playfullscreenrelation.h"
#include "widget/myqwidget.h"//c220730

#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QMutex>


namespace music {

    /**
     * @brief The MusicMain class 음악 메인메뉴의 총괄 @n
     *      음악 대항목 관련 모든 QWidget 관리
     */
    class MusicMain : public AbstractMainContent
    {
        Q_OBJECT
    public:
        explicit MusicMain(QWidget *parent = nullptr);
        ~MusicMain();

    signals:

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

    private slots:
        void slot_goFolder(QString str);//c220909_1
        void slot_dragEnterEvent_restore();//c220826_1
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;//cheon211015

        void slot_clickedViewAll(QString p_viewAllMode);
        void slot_clickedViewAll_Recommand(QString p_viewAllMode, QJsonObject jsonObj);
        void slot_clickedViewAllForSearch(const QString &p_text, const QString &p_viewAllMode);
        void slot_clickedFavoriteFolderEdit(const QJsonArray &p_jsonArrFavor);
        void slot_search(const QString &p_text);
        void slot_totalsearch();//cheon211008
        //void slot_showPlayFullScreen(const QJsonObject &p_jsonObject);

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

    private:
        void setInit();
        void setUIControl();

    private :
        // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요

        QWidget *curr_widget;//c220730
        // 메뉴 관련 페이지  ----------------------
        MusicHome *musicHome=nullptr;                        ///< 메뉴 : 음악 > 홈
        MusicCategory *musicCategory=nullptr;                       ///< 메뉴 : 음악 > 분류
        MusicAlbum *musicAlbum=nullptr;                             ///< 메뉴 : 음악 > 앨범
        MusicFolder *musicFolder=nullptr;                           ///< 메뉴 : 음악 > 폴더
        MusicMyCollection *musicMyCollection = nullptr;      ///< 메뉴 : 음악 > My Collection
        MusicSearch *musicSearch=nullptr;                    ///< 음악 > 검색
        MusicFavorite *musicFavorite=nullptr;                ///< 메뉴 : 음악 > My Collection > Favorites
        Musice_playlist *musice_playlist=nullptr;                   ///< 플레이리스트 상세
        //MusicCollection *musicCollection=nullptr;                 ///< 메뉴 : 음악 > My Collection
        //MusicSearch *musicSearch=nullptr;                         ///< 음악 > 검색

        // 옵션팝업 관련 페이지 ----------------------
        //AlbumDetail *albumDetail = nullptr;                       ///< 옵션팝업 : 앨범상세
        AlbumDetail *albumDetail = nullptr;                  ///< 옵션팝업 : 앨범상세
        AlbumDetail_Rose *albumDetail_rose = nullptr;        ///< 옵션팝업 : 앨범상세 for Rose
        AlbumListAll *albumListAll = nullptr;                ///< 옵션팝업 : 앨범리스트
        PlaylistDetail_Rose *playlistDetail_rose = nullptr;  ///< 옵션팝업 : 플레이리스트상세 for Rose
        PlaylistAll *playlistListAll = nullptr;              ///< 옵션팝업 : 플레이리스트 리스트
        MyPlaylistAll *myplaylistListAll = nullptr;          ///< 옵션팝업 : 마이플레이리스트 리스트
        UserPlaylistAll *userplaylistListAll = nullptr;      ///< 옵션팝업 : 유저플레이리스트 리스트
        TrackListAll *trackListAll_rose = nullptr;           ///< 옵션팝업 : 트랙리스트
        ArtistDetail *artistDetail = nullptr;                ///< 옵션팝업 : 아티스트상세
        ArtistListAll *artistListAll = nullptr;              ///< 옵션팝업 : 아티스트리스트
        ArtistAlbumListAll *artistAlbumListAll = nullptr;    ///< 옵션팝업 : 아티스트 앨범리스트
        ArtistTrackListAll *artistTrackListAll = nullptr;    ///< 옵션팝업 : 아티스트 트랙리스트
        SearchAlbumListAll *searchAlbumListAll = nullptr;    ///< 옵션팝업 : 앨범리스트
        SearchTrackListAll *searchTrackListAll = nullptr;    ///< 옵션팝업 : 트랙리스트
        MetaInfo *metaInfo=nullptr;                                 ///< 옵션팝업 : 메타정보
        ArtistInfo *artistInfo=nullptr;                             ///< 옵션팝업 : 아티스트상세

        GenreInfo *genreInfo=nullptr;                               ///< 메뉴 : 음악-분류-장르상세
        ComposerInfo *composerInfo=nullptr;                         ///< 메뉴 : 음악-분류-작곡가상세

        // 기타 관련 페이지 ----------------------

        MusicFolder_EditFavor *musicFavoriteEdit=nullptr;           ///< 즐겨찾기 편집
        //PlayFullScreenRelation *playFullScreenRelation;           ///< 재생화면 (연관 컨텐츠 있는)

        // View All 관련 페이지 ----------------------
        MusicList_Group *viewAll_playAlbum=nullptr;
        MusicList_Group *viewAll_playPlaylist=nullptr;
        MusicList_Group *viewAll_addedAlbum=nullptr;
        MusicList_Group *viewAll_myPlayList=nullptr;
        MusicList_Group *viewAll_friendPlayList=nullptr;
        MusicList_Group *viewAll_recommendArtist=nullptr;
        MusicList_Group *viewAll_searchAlbum=nullptr;
        MusicList_Group *viewAll_searchArtist=nullptr;
        MusicList_One *viewAll_searchTrack=nullptr;
        MusicList_CollectionTrack *viewAll_collectionTrack=nullptr;
        MusicList_ArtistTrack *viewAll_artistTrack=nullptr;
        MusicList_ArtistAlbum *viewAll_artistAlbum=nullptr;
        MusicList_GenreAlbum *viewAll_genreAlbum=nullptr;

        // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요
    };
};

#endif // MUSICMAIN_H
