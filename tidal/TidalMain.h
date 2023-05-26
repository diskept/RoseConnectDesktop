#ifndef TIDALMAIN_H
#define TIDALMAIN_H


#include "tidal/TidalHome.h"              // 홈
#include "tidal/TidalVideo.h"             // 비디오
#include "tidal/TidalMaster.h"            // Master
#include "tidal/TidalSettings.h"          // 설정

// 탐색
#include "tidal/TidalExplore.h"
#include "tidal/TidalGenresMain.h"
#include "tidal/TidalGenresChoose.h"
#include "tidal/TidalMoodsMain.h"
#include "tidal/TidalNewMain.h"
#include "tidal/TidalTop20Main.h"
#include "tidal/TidalRisingMain.h"

// 공통
#include "tidal/TidalPlaylistAll.h"
#include "tidal/TidalPlaylistDetail.h"
#include "tidal/TidalAlbumListAll.h"
#include "tidal/TidalTracksListAll.h"
#include "tidal/TidalTrackListAll_Share.h"
#include "tidal/TidalAlbumDetail.h"
#include "tidal/TidalMyTrackAll.h"
#include "tidal/TidalArtistListAll.h"
#include "tidal/TidalArtistDetail.h"
#include "tidal/TidalVideoListAll.h"
#include "tidal/TidalAddPlaylist.h"
#include "tidal/TidalHistoryListAll.h"
#include "tidal/TidalHistoryDetail.h"
#include "tidal/TidalRecentlyListDelete.h"

// My Collection
#include "tidal/TidalMyCollection.h"
#include "tidal/TidalMyPlaylistAll.h"
#include "tidal/TidalMyTidalPlaylistAll.h"
#include "tidal/TidalMyPlaylistDetail.h"
#include "tidal/TidalMyPlaylistEdit.h"
#include "tidal/TidalMixesDetail.h"
#include "tidal/TidalMyAlbumAll.h"
#include "tidal/TidalMyArtistAll.h"
#include "tidal/TidalMyVideoAll.h"

// Search
#include "tidal/TidalSearchMain.h"
#include "tidal/TidalSearchTrackAll.h"
#include "tidal/TidalSearchArtistAll.h"
#include "tidal/TidalSearchAlbumAll.h"
#include "tidal/TidalSearchPlaylistAll.h"
#include "tidal/TidalSearchVidelAll.h"


#include <QJsonObject>
#include <QWidget>
#include <QMutex>
#include <home/abstractmaincontent.h>
#include "widget/myqwidget.h"//c220730


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal Main 클래스. (서브 페이지들을 관할하는 @sa AbstractMainContent 자식 클래스)
     */
    class TidalMain : public AbstractMainContent
    {
        Q_OBJECT
    public:
        explicit TidalMain(QWidget *parent = nullptr);
        void process_searchWord(const QString&) override;

    signals:
        void successLogin();

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

    private slots:
        void slot_loginAcount();//c230426
        void slot_dragEnterEvent_restore();//c220826_1
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;//cheon211015

        void slot_clickedMoveSubPage(const QJsonObject &p_jsonData);

        void slot_search(const QString&);

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;
        void resizeEvent(QResizeEvent *event) override;


    private :
        // UI 세팅
        void setUIControl();

        // 데이터 반환
        QJsonObject getJsonObject_forMainTitle();
        QJsonArray getJsonArray_forSubTitles();


        // 페이지 이동 처리
        template<typename T_TidalSub>
        T_TidalSub* procCommon_showSubPage(const bool flagShow_topBar, T_TidalSub *sub_widget, const QJsonObject& jsonObj_data);
        void animate_fadeOut_forSubWidget(QWidget *p_widget);
        void animate_fadeIn_forSubWidget(QWidget *p_widget);

        QWidget *curr_widget;//c220730

        // 메뉴 관련 페이지  ---------------------- START
        TidalHome *sub_home = nullptr;                                  ///< 타이달 > 홈

        TidalExplore *sub_explore = nullptr;                            ///< 타이달 > 탐색
        TidalGenresMain *sub_genresMain = nullptr;                      ///< 타이달 > 탐색 하위 > 장르 메인
        TidalGenresChoose *sub_genresChoose = nullptr;                  ///< 타이달 > 탐색 하위 > 장르 선택
        TidalMoodsMain *sub_moodsMain = nullptr;                        ///< 타이달 > 탐색 하위 > 무드 메인
        TidalNewMain *sub_newMain = nullptr;                            ///< 타이달 > 탐색 > New 메인
        TidalTop20Main *sub_topMain = nullptr;                          ///< 타이달 > 탐색 > Top 메인
        TidalRisingMain *sub_risingMain = nullptr;                      ///< 타이달 > 탐색 > TIDAL Rising
        TidalVideo *sub_video = nullptr;                                ///< 타이달 > 비디오
        TidalMaster *sub_master = nullptr;                              ///< 타이달 > 마스터
        TidalSettings *sub_settings = nullptr;                          ///< 타이달 > 설정

        // 공통
        TidalPlaylistDetail *sub_playlistDetail = nullptr;              ///< 타이달 전용 Playlist 상세 화면
        TidalMixesDetail *sub_mixesDetail = nullptr;                    ///< 타이달 전용 Mixes 상세 화면
        TidalAlbumDetail *sub_albumDetail = nullptr;                    ///< 타이달 전용 Album 상세 화면 (ListWidget 기반)
        TidalTracksListAll *sub_tracksListAll = nullptr;                ///< 타이달 트랙 목록 전체 (ListWidget 기반)
        TidalTrackListAll_Share *sub_trackListAll_Share = nullptr;      ///< 타이달 공유 트랙 목록
        TidalPlaylistAll *sub_playlistAll = nullptr;                    ///< 타이달 플레이리스트 전체 (flowLayout 기반)
        TidalAlbumListAll *sub_albumAll = nullptr;                      ///< 타이달 앨범 목록 전채 (flowLayout 기반)
        TidalArtistListAll *sub_artistAll = nullptr;                    ///< 타이달 Artist 목록 전체 (flowLayout 기반)
        TidalArtistDetail *sub_artistDetail = nullptr;                  ///< 타이달 전용 Artist 상세 화면
        TidalVideoListAll *sub_videoAll = nullptr;                      ///< 타이달 Video 전체 (flowLayout 기반)
        TidalAddPlaylist *sub_addPlaylist = nullptr;                    ///< 타이달 Playlist 추가/삭제/변경 화면
        TidalHistoryListAll *sub_historyListAll = nullptr;
        TidalHistoryDetail *sub_historyDetail = nullptr;
        TidalPlaylistHistoryAll *sub_historyPlaylistAll = nullptr;
        TidalAlbumHistoryAll *sub_historyAlbumAll = nullptr;
        TidalTrackHistoryAll *sub_historyTrackAll = nullptr;
        TidalArtistHistoryAll *sub_historyArtistAll = nullptr;
        TidalRecentlyListDelete *sub_recentlyListDelete = nullptr;

        // My Collection
        TidalMycollection *sub_myCollection = nullptr;                  ///< 타이달 > My Collection
        TidalMyTidalPlaylistAll *sub_myTidalPlaylistAll = nullptr;      ///< 타이달 > My Collection > Tidal Playlists
        TidalMyPlaylistAll *sub_myPlaylistAll = nullptr;                ///< 타이달 > My Collection > My Playlists
        TidalMyPlaylistDetail *sub_myPlaylistDetail = nullptr;          ///< 타이달 > My Collection > My Playlists > 상세
        TidalMyPlaylistEdit *sub_myPlaylistEdit = nullptr;              ///< 타이달 > My Collection > My Playlists > 편집
        TidalMyAlbumAll *sub_myAlbumAll = nullptr;                      ///< 타이달 > My Collection > My Favorite Albums
        TidalMyArtistAll *sub_myArtistAll = nullptr;                    ///< 타이달 > My Collection > My Favorite Artist
        TidalMyTrackAll *sub_myTrackAll = nullptr;                      ///< 타이달 > My Collection > My Favorite Tracks
        TidalMyVideoAll *sub_myVideoAll = nullptr;                      ///< 타이달 > My Collection > My Favorite Videos
        TidalMyRosePlaylistAll *sub_myRosePlaylistAll = nullptr;        ///< 타이달 > 홈 > My Playlists
        TidalUserRosePlaylistAll *sub_userRosePlaylistAll = nullptr;    ///< 타이달 > 홈 > User Playlists
        TidalMyPlaylistDetail_ROSE *sub_myRosePlaylistDetail = nullptr; ///< 타이달 > 홈 > My Playlists > 상세
        TidalRecentlyAlbumAll *sub_recentlyAlbumAll = nullptr;          ///< 타이달 > 홈 > my Recently Albums
        TidalRecentlyPlaylistAll *sub_recentlyPlaylistAll = nullptr;    ///< 타이달 > 홈 > My Recently Playlists
        TidalRecentlyTrackAll *sub_recentlyTrackAll = nullptr;          ///< 타이달 > 홈 > My Recently Tracks
        TidalRecenltyArtistAll *sub_recentlyArtistAll = nullptr;        ///< 타이달 > 홈 > My Recently Artists

        // Search
        TidalSearchMain *sub_searchMain = nullptr;                      ///< 타이달 > 검색 메인
        TidalSearchTrackAll *sub_searchTrackAll = nullptr;              ///< 타이달 > 검색 > Track All View
        TidalSearchArtistAll *sub_searchArtistAll = nullptr;            ///< 타이달 > 검색 > Artist All View
        TidalSearchAlbumAll *sub_searchAlbumAll = nullptr;              ///< 타이달 > 검색 > Album All View
        TidalSearchPlaylistAll *sub_searchPlaylistAll = nullptr;        ///< 타이달 > 검색 > Playlist All View
        TidalSearchVidelAll *sub_searchVideoAll = nullptr;              ///< 타이달 > 검색 > Video All View

        // 메뉴 관련 페이지  ---------------------- END


    };

};

#endif // TIDALMAIN_H
