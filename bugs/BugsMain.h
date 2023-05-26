#ifndef BUGSMAIN_H
#define BUGSMAIN_H

#include "bugs/BugsHome.h"           // Home
#include "bugs/BugsMusicRecently.h"  // Home > 최신 음악
#include "bugs/BugsMusicChart.h"     // Home > 벅스 차트

#include "bugs/BugsExplore.h"        // 탐색
#include "bugs/BugsThemesChoose.h"   // 탐색 > 장르/테마 선택


#include "bugs/BugsMyCollection.h"   // My Collection
#include "bugs/BugsMyLikeMusic.h"    // My Collection > 좋아한 음악

#include "bugs/BugsSettings.h"       // 설정

#include "bugs/BugsTracksListAll.h"              // Track All View (General)
#include "bugs/BugsTrackListAll_Share.h"         // Track Share All View
#include "bugs/BugsAlbumListAll_ofArtist.h"      // Album All View - of the Artist
#include "bugs/BugsTrackListAll_ofArtist.h"      // Track All View - of the Artist
#include "bugs/BugsPDAlbumListAll.h"             // PD_Album All View (General)
#include "bugs/BugsVideoList_withTrack.h"        // Video All View with the Track
#include "bugs/BugsHistoryListAll.h"
#include "bugs/BugsAlbumDetail.h"        // Album 상세
#include "bugs/BugsArtistDetail.h"       // Artist 상세
#include "bugs/BugsPDAlbumDetail.h"      // PD Album 상세
#include "bugs/BugsMyAlbumDetail.h"      // My Album 상세
#include "bugs/BugsHistoryDetail.h"

#include "bugs/BugsSearchMain.h"         // 검색 메인
#include "bugs/BugsSearchTrackAll.h"     // 검색 - Track All View
#include "bugs/BugsSearchArtistAll.h"    // 검색 - Artist All View
#include "bugs/BugsSearchAlbumAll.h"     // 검색 - Album All View
#include "bugs/BugsSearchPD_AlbumAll.h"  // 검색 - PD's Album All View
#include "bugs/BugsSearchVideoAll.h"     // 검색 - Video All View

#include "bugs/BugsMyAlbumAll.h"
#include "bugs/BugsMyArtistAll.h"
#include "bugs/BugsMyPlaylistAll.h"
#include "bugs/BugsMyTrackAll.h"
#include "bugs/BugsMyVideoAll.h"
#include "bugs/BugsMyPlaylistDetail.h"
#include "bugs/BugsAddPlaylist.h"
#include "bugs/BugsRecentlyListDelete.h"

#include <QJsonObject>
#include <QWidget>
#include <QMutex>
#include <home/abstractmaincontent.h>
#include "widget/myqwidget.h"//c220730


/*
// M.V.R.RT.P.T.[B].SP.S
const QString PAGECODE_BUGS___HOME = "BUGS___HOME";
*/


namespace bugs {

    /**
     * @brief 벅스 Main 클래스. (서브 페이지들을 관할하는 @sa AbstractMainContent 자식 클래스)
     */
    class BugsMain : public AbstractMainContent
    {
        Q_OBJECT
    public:
        explicit BugsMain(QWidget *parent = nullptr);
        void process_searchWord(const QString &p_text) override;

    signals:

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

    private slots:
        void slot_loginAcount();//c230426
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;//cheon211015

        void slot_clickedMoveSubPage(const QJsonObject &p_jsonData);

        void slot_search(const QString&);
        void slot_dragEnterEvent_restore();//c220826_1

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;


    private :
        // UI 세팅
        void setUIControl();

        // 데이터 반환
        void request_login_getMemberInfo();
        QJsonObject getJsonObject_forMainTitle();
        QJsonArray getJsonArray_forSubTitles();


        // 페이지 이동 처리
        template<typename T_BugsSub>
        T_BugsSub* procCommon_showSubPage(const bool flagShow_topBar, T_BugsSub *sub_widget, const QJsonObject& jsonObj_data);
        QWidget *curr_widget;//c220730

        // 메뉴 관련 페이지  ---------------------- START
        BugsHome *sub_home = nullptr;                       ///< 벅스 > 홈
        BugsMusicRecently *sub_musicRecently = nullptr;     ///< 벅스 > 홈 > 최신음악
        BugsMusicChart *sub_musicChart = nullptr;           ///< 벅스 > 홈 > 벅스차트
        BugsTrackRecentlyAll *sub_trackRecentlyAll = nullptr;
        BugsAlbumRecentlyAll *sub_albumRecentlyAll = nullptr;
        BugsVideoRecentlyAll *sub_videoRecentlyAll = nullptr;
        BugsTrackChartAll *sub_trackChartAll = nullptr;
        BugsAlbumChartAll *sub_albumChartAll = nullptr;
        BugsVideoChartAll *sub_videoChartAll = nullptr;

        // Rose Server        
        BugsRecentlyAlbumAll *sub_recentlyAlbumAll = nullptr;
        BugsRecentlyPlaylistAll *sub_recentlyPlaylistAll = nullptr;
        BugsRecentlyTrackAll *sub_recentlyTrackAll = nullptr;
        BugsRecenltyArtistAll *sub_recentlyArtistAll = nullptr;
        BugsPlaylistDetail *sub_playlistDetail = nullptr;
        BugsMyRosePlaylistAll *sub_myRosePlaylistAll = nullptr;
        BugsUserRosePlaylistAll *sub_userRosePlaylistAll = nullptr;
        BugsHistoryListAll *sub_historyListAll = nullptr;
        BugsHistoryDetail *sub_historyDetail = nullptr;
        BugsPlaylistHistoryAll *sub_historyPlaylistAll = nullptr;
        BugsAlbumHistoryAll *sub_historyAlbumAll = nullptr;
        BugsTrackHistoryAll *sub_historyTrackAll = nullptr;
        BugsArtistHistoryAll *sub_historyArtistAll = nullptr;
        BugsAddPlaylist *sub_AddPlaylist = nullptr;                         ///< 벅스 > Playlist 추가/삭제/변경 화면
        BugsRecentlyListDelete *sub_roseRecentlyDelete = nullptr;

        BugsExplore *sub_explore = nullptr;                                 ///< 벅스 > 탐색
        BugsThemesChoose *sub_themeChoose = nullptr;                        ///< 벅스 > 탐색 > 장르/테마 선택

        BugsMyCollection *sub_myCollection = nullptr;                       ///< 벅스 > My Collection
        BugsMyLikeMusic *sub_myLikeMusic = nullptr;                         ///< 벅스 > My Collection > 좋아한 음악
        BugsMyAlbumlistAll *sub_myAlbumlist = nullptr;                        ///< 벅스 > My Collection > 내 앨범

        BugsSettings *sub_settings = nullptr;                               ///< 벅스 > 설정

        BugsArtistDetail *sub_artistDetail = nullptr;                       ///< 벅스 > Artist 상세
        BugsAlbumDetail *sub_albumDetail = nullptr;                         ///< 벅스 > Album 상세
        BugsPDAlbumDetail *sub_pd_albumDetail = nullptr;                    ///< 벅스 > PD's Album 상세
        BugsMyAlbumDetail *sub_my_albumDetail = nullptr;                    ///< 벅스 > My Album 상세 (user's playlist)

        BugsTracksListAll *sub_trackAll = nullptr;                          ///< 벅스 > Track All View
        BugsMostTracksListAll *sub_mostTrackAll = nullptr;                  ///< 벅스 > Track All View
        BugsTrackListAll_Share *sub_trackAll_Share = nullptr;               ///< 벅스 > Share Track All View
        BugsAlbumListAll_ofArtist *sub_albumAll_ofArtist = nullptr;         ///< 벅스 > Artist의 Album All View
        BugsTrackListAll_ofArtist *sub_trackAll_ofArtist = nullptr;         ///< 벅스 > Artist의 Track All View
        BugsPDAlbumListAll *sub_pd_albumAll = nullptr;                      ///< 벅스 > PD's Album All View
        BugsVideoList_withTrack *sub_videoAll_withTrack = nullptr;          ///< 벅스 > Video All View (with the Track)

        BugsSearchMain *sub_searchMain = nullptr;                           ///< 검색 > 메인
        BugsSearchTrackAll *sub_searchTrackAll = nullptr;                   ///< 검색 > Track All View
        BugsSearchArtistAll *sub_searchArtistAll = nullptr;                 ///< 검색 > Artist All View
        BugsSearchAlbumAll *sub_searchAlbumAll = nullptr;                   ///< 검색 > Album All View
        BugsSearchPD_AlbumAll *sub_searchPD_AlbumAll = nullptr;             ///< 검색 > PD's Album All View
        BugsSearchVideoAll *sub_searchVideoAll = nullptr;                   ///< 검색 > Video All View

//        // 메뉴 관련 페이지  ---------------------- END


    };

};

#endif // BUGSMAIN_H
