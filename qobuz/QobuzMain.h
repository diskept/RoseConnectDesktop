#ifndef QOBUZMAIN_H
#define QOBUZMAIN_H


#include "qobuz/QobuzHome.h"              // 홈
#include "qobuz/QobuzVideo.h"             // 비디오
#include "qobuz/QobuzMaster.h"            // Master
#include "qobuz/QobuzSettings.h"          // 설정

// 탐색
#include "qobuz/QobuzExplore.h"
#include "qobuz/QobuzGenresMain.h"
#include "qobuz/QobuzGenresChoose.h"
#include "qobuz/QobuzMoodsMain.h"
#include "qobuz/QobuzNewMain.h"
#include "qobuz/QobuzTop20Main.h"
#include "qobuz/QobuzRisingMain.h"

// 공통
#include "qobuz/QobuzPlaylistAll.h"
#include "qobuz/QobuzPlaylistDetail.h"
#include "qobuz/QobuzAlbumListAll.h"
#include "qobuz/QobuzTracksListAll.h"
#include "qobuz/QobuzTrackListAll_Share.h"
#include "qobuz/QobuzAlbumDetail.h"
#include "qobuz/QobuzMyTrackAll.h"
#include "qobuz/QobuzArtistListAll.h"
#include "qobuz/QobuzArtistDetail.h"
#include "qobuz/QobuzVideoListAll.h"
#include "qobuz/QobuzHistoryListAll.h"
#include "qobuz/QobuzHistoryDetail.h"

// My Collection
#include "qobuz/QobuzMyCollection.h"
#include "qobuz/QobuzMyFavorites.h"
#include "qobuz/QobuzMyPurchases.h"
#include "qobuz/QobuzMyPlaylistAll.h"
#include "qobuz/QobuzMyPlaylistDetail.h"
#include "qobuz/QobuzMyPlaylistEdit.h"
#include "qobuz/QobuzMyAlbumAll.h"
#include "qobuz/QobuzMyArtistAll.h"
#include "qobuz/QobuzMyVideoAll.h"
#include "qobuz/QobuzAddPlaylist.h"
#include "qobuz/QobuzRecentlyListDelete.h"

// Search
#include "qobuz/QobuzSearchMain.h"
#include "qobuz/QobuzSearchTrackAll.h"
#include "qobuz/QobuzSearchArtistAll.h"
#include "qobuz/QobuzSearchAlbumAll.h"
#include "qobuz/QobuzSearchPlaylistAll.h"
#include "qobuz/QobuzSearchVidelAll.h"

#include "home/abstractmaincontent.h"
#include "widget/myqwidget.h"//c220730

#include <QJsonObject>
#include <QWidget>


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz Main 클래스. (서브 페이지들을 관할하는 @sa AbstractMainContent 자식 클래스)
     */
    class QobuzMain : public AbstractMainContent
    {
        Q_OBJECT
    public:
        explicit QobuzMain(QWidget *parent = nullptr);
        void process_searchWord(const QString &p_text) override;

    signals:
        void successLogin();

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

    private slots:
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
        template<typename T_QobuzSub>
        T_QobuzSub* procCommon_showSubPage(const bool flagShow_topBar, T_QobuzSub *sub_widget, const QJsonObject& jsonObj_data);
        void animate_fadeOut_forSubWidget(QWidget *p_widget);
        void animate_fadeIn_forSubWidget(QWidget *p_widget);

        QWidget *curr_widget;//c220730

        // 메뉴 관련 페이지  ---------------------- START
        QobuzHome *sub_home = nullptr;                                  ///< 코부즈 > 홈
        QobuzExplore *sub_explore = nullptr;                            ///< 코부즈 > 탐색
        QobuzVideo *sub_video = nullptr;
        QobuzMaster *sub_master = nullptr;
        QobuzSettings *sub_settings = nullptr;                          ///< 코부즈 > 설정

        QobuzGenresMain *sub_genresMain = nullptr;                      ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresPop = nullptr;                       ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresJazz = nullptr;                      ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresClassic = nullptr;                   ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresElectro = nullptr;                   ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresSoul = nullptr;                      ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresFolk = nullptr;                      ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresHiphop = nullptr;                    ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresCountry = nullptr;                   ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresMetal = nullptr;                     ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresBlues = nullptr;                     ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresLatin = nullptr;                     ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresSoundtracks = nullptr;               ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresWorld = nullptr;                     ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresMain *sub_genresOther = nullptr;                     ///< 코부즈 > 탐색 하위 > 장르 메인
        QobuzGenresChoose *sub_genresChoose = nullptr;                  ///< 코부즈 > 탐색 하위 > 장르 선택
        QobuzMoodsMain *sub_moodsMain = nullptr;                        ///< 코부즈 > 탐색 하위 > 무드 메인
        QobuzNewMain *sub_newMain = nullptr;                            ///< 코부즈 > 탐색 > New 메인
        QobuzTop20Main *sub_topMain = nullptr;                          ///< 코부즈 > 탐색 > Top 메인
        QobuzRisingMain *sub_risingMain = nullptr;                      ///< 코부즈 > 탐색 > QOBUZ Rising


        // 공통
        QobuzPlaylistDetail *sub_playlistDetail = nullptr;              ///< 코부즈 전용 Playlist 상세 화면
        QobuzAlbumDetail *sub_albumDetail = nullptr;                    ///< 코부즈 전용 Album 상세 화면 (ListWidget 기반)
        QobuzTracksListAll *sub_tracksListAll = nullptr;                ///< 코부즈 트랙 목록 전체 (ListWidget 기반)
        QobuzTrackListAll_Share *sub_trackListAll_Share = nullptr;      ///< 코부즈 공유 트랙 목록 전체
        QobuzAppearsonListAll *sub_appearsonListAll = nullptr;          ///< 코부즈 appears no 목록 전체 (ListWidget 기반)
        QobuzPlaylistAll *sub_playlistAll = nullptr;                    ///< 코부즈 플레이리스트 전체 (flowLayout 기반)
        QobuzSimilarplaylistAll *sub_similarlistAll = nullptr;          ///< 코부즈 similar Playlist 목록 (flowLayout 기반)
        QobuzAlbumListAll *sub_albumAll = nullptr;                      ///< 코부즈 앨범 목록 전채 (flowLayout 기반)
        QobuzAlbumListAll *sub_classicAll = nullptr;                    ///< 코부즈 앨범 목록 전채 (flowLayout 기반)
        QobuzAlbumListAll *sub_idealDiscographyAll = nullptr;           ///< 코부즈 앨범 목록 전채 (flowLayout 기반)
        QobuzAlbumListAll *sub_stillTrendingAll = nullptr;              ///< 코부즈 앨범 목록 전채 (flowLayout 기반)
        QobuzAlbumListAll *sub_pressAwardsAll = nullptr;                ///< 코부즈 앨범 목록 전채 (flowLayout 기반)
        QobuzAlbumListAll *sub_qobuzIssimeAll = nullptr;                ///< 코부즈 앨범 목록 전채 (flowLayout 기반)
        QobuzAlbumListAll *sub_topReleaseAll = nullptr;                 ///< 코부즈 앨범 목록 전채 (flowLayout 기반)
        QobuzLastReleaseList *sub_lastRelease = nullptr;                ///< 코부즈 Last Release 목록 (flowLayout 기반)
        QobuzSameArtistAll *sub_sameArtistAll = nullptr;                ///< 코부즈 same Artist 목록 (flowLayout 기반)
        QobuzArtistListAll *sub_artistAll = nullptr;                    ///< 코부즈 Artist 목록 전체 (flowLayout 기반)
        QobuzArtistDetail *sub_artistDetail = nullptr;                  ///< 코부즈 전용 Artist 상세 화면
        QobuzVideoListAll *sub_videoAll = nullptr;                      ///< 코부즈 Video 전체 (flowLayout 기반)
        QobuzAddPlaylist *sub_AddPlaylist = nullptr;                    ///< 코부즈 Playlist 추가/삭제/변경 화면
        QobuzHistoryListAll *sub_historyListAll = nullptr;
        QobuzHistoryDetail *sub_historyDetail = nullptr;
        QobuzPlaylistHistoryAll *sub_historyPlaylistAll = nullptr;
        QobuzAlbumHistoryAll *sub_historyAlbumAll = nullptr;
        QobuzTrackHistoryAll *sub_historyTrackAll = nullptr;
        QobuzArtistHistoryAll *sub_historyArtistAll = nullptr;

        // My Collection
        QobuzMycollection *sub_myCollection = nullptr;                  ///< 코부즈 > My Collection
        QobuzMyPlaylistAll *sub_myPlaylistAll = nullptr;                ///< 코부즈 > My Collection > My Playlists
        QobuzMyPlaylistDetail *sub_myPlaylistDetail = nullptr;          ///< 코부즈 > My Collection > My Playlists > 상세
        QobuzMyPlaylistEdit *sub_myPlaylistEdit = nullptr;              ///< 코부즈 > My Collection > My Playlists > 편집
        QobuzMyFavorites *sub_myFavorites = nullptr;                    ///< 코부즈 > My Collection > My Favorite
        QobuzMyAlbumAll *sub_myAlbumAll = nullptr;                      ///< 코부즈 > My Collection > My Favorite Albums
        QobuzMyArtistAll *sub_myArtistAll = nullptr;                    ///< 코부즈 > My Collection > My Favorite Artist
        QobuzMyTrackAll *sub_myTrackAll = nullptr;                      ///< 코부즈 > My Collection > My Favorite Tracks
        QobuzMyVideoAll *sub_myVideoAll = nullptr;                      ///< 코부즈 > My Collection > My Favorite Videos
        QobuzMyPurchases *sub_myPurchases = nullptr;                    ///< 코부즈 > My Purchases
        QobuzMyPurchasesAlbumAll *sub_myPurchasesAlbumAll = nullptr;    ///< 코부즈 > My Purchases > My Favorite Albums
        QobuzMyPurchasesTrackAll *sub_myPurchasesTrackAll = nullptr;    ///< 코부즈 > My Purchases > My Favorite Tracks

        QobuzRecentlyAlbumAll *sub_recentlyAlbumAll = nullptr;          ///< 코부즈 > 홈 > My Recently Albums
        QobuzRecentlyPlaylistAll *sub_recentlyPlaylistAll = nullptr;    ///< 코부즈 > 홈 > My Recently Playlists
        QobuzRecentlyTrackAll *sub_recentlyTrackAll = nullptr;          ///< 코부즈 > 홈 > My Recently Tracks
        QobuzRecenltyArtistAll *sub_recenltyArtistAll = nullptr;        ///< 코부즈 > 홈 > My Recently Artists
        QobuzRecentlyListDelete *sub_recentlyListDelete = nullptr;      ///< 코부즈 > 홈 > My Recently list Delete
        QobuzMyPlaylistDetail_ROSE *sub_myRosePlaylistDetail = nullptr; ///< 코부즈 > 홈 > My Playlists > 상세
        QobuzMyRosePlaylistAll *sub_myRosePlaylistAll = nullptr;        ///< 코부즈 > 홈 > My Playlists
        QobuzUserRosePlaylistAll *sub_userRosePlaylistAll = nullptr;    ///< 코부즈 > 홈 > User Playlists

        // Search
        QobuzSearchMain *sub_searchMain = nullptr;                      ///< 코부즈 > 검색 메인
        QobuzSearchTrackAll *sub_searchTrackAll = nullptr;              ///< 코부즈 > 검색 > Track All View
        QobuzSearchArtistAll *sub_searchArtistAll = nullptr;            ///< 코부즈 > 검색 > Artist All View
        QobuzSearchAlbumAll *sub_searchAlbumAll = nullptr;              ///< 코부즈 > 검색 > Album All View
        QobuzSearchPlaylistAll *sub_searchPlaylistAll = nullptr;        ///< 코부즈 > 검색 > Playlist All View
        QobuzSearchVidelAll *sub_searchVideoAll = nullptr;              ///< 코부즈 > 검색 > Video All View



        // 메뉴 관련 페이지  ---------------------- END


    };

};

#endif // QOBUZMAIN_H
