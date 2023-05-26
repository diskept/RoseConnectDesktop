#ifndef ROSEHOMEMAIN_H
#define ROSEHOMEMAIN_H

#include "roseHome/rosehomeHome.h"
#include "roseHome/rosehomeFavorite.h"
#include "roseHome/rosehomeUserPick.h"
#include "roseHome/userpickUserpage.h"
#include "roseHome/rosehomeFriend.h"

#include "roseHome/rosehomeAlbumDetail.h"
#include "roseHome/rosehomeAlbumListAll.h"
#include "roseHome/rosehomeArtistDetail.h"
#include "roseHome/rosehomeArtistListAll.h"
#include "roseHome/rosehomePlaylistDetail.h"
#include "roseHome/rosehomePlaylistDetail_Rose.h"
#include "roseHome/rosehomeHistoryDetail.h"
#include "roseHome/rosehomePlaylistListAll.h"
#include "roseHome/rosehomeTrackListAll.h"
#include "roseHome/rosehomeTrackListAll_Share.h"
#include "roseHome/rosehomeRosetubeListAll.h"
#include "roseHome/rosehomeHistoryListAll.h"
#include "roseHome/rosehomeAddPlaylist.h"
#include "roseHome/rosehomeRecentlyListDelete.h"

#include "home/abstractmaincontent.h"
#include "widget/myqwidget.h"//c220730

#include <QJsonObject>
#include <QWidget>
#include <QMutex>


namespace roseHome {
    /**
     * @brief roseHome Main 클래스. (서브 페이지들을 관할하는
     * @sa AbstractMainContent 자식 클래스)
     */
    class roseHomeMain : public AbstractMainContent
    {
        Q_OBJECT

    public:
        explicit roseHomeMain(QWidget *parent = nullptr);
        ~roseHomeMain();

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;//cheon211015

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

    private slots:
        void slot_dragEnterEvent_restore();//c220826_1
        void slot_clickedMoveSubPage(const QJsonObject &p_jsonData);


    private:
        void requestGetRoseHome_forUserpickInfo();//cheon211015

    private :
        Linker *linker;

        // UI 세팅
        void setUIControl();

        // 데이터 반환
        QJsonObject getJsonObject_forMainTitle();
        QJsonArray getJsonArray_forSubTitles();

        // 페이지 이동 처리
        template<typename T_roseHomeSub>
        T_roseHomeSub* procCommon_showSubPage(const bool flagShow_topBar, T_roseHomeSub *sub_widget, const QJsonObject& jsonObj_data);

        QWidget *curr_widget;//c220730
        // 메뉴 관련 페이지  ---------------------- START
        RoseHome *sub_home = nullptr;                                       ///< 로즈홈 > 홈
        RoseFavorite *sub_favorite = nullptr;                               ///< 로즈홈 > Favorite
        RoseHomeUserPick *sub_userpick = nullptr;                           ///< 로즈홈 > User Pick
        RoseHomeFriend *sub_friend = nullptr;                               ///< 로즈홈 > Friends
        userpickUserpage *sub_userpage = nullptr;

        // 공통
        RoseHomeAlbumDetail *sub_albumDetail = nullptr;                     ///< 로즈홈 앨범 상세 화면 (ListWidget 기반)
        RoseHomeAlbumListAll *sub_albumAll = nullptr;                       ///< 로즈홈 앨범 목록 전체 (flowLayout 기반)
        RoseHomeArtistDetail *sub_artistDetail = nullptr;                   ///< 로즈홈 아티스트 상세 화면 (ListWidget 기반)
        RoseHomeArtistListAll *sub_artistAll = nullptr;                     ///< 로즈홈 아티스트 목록 전체 (flowLayout 기반)
        RoseHomePlaylistDetail *sub_playlistDetail = nullptr;               ///< 로즈홈 플레이리스트 상세 화면 (ListWidget 기반)
        RoseHomePlaylistDetail_Rose *sub_playlistDetail_rose = nullptr;     ///< 로즈홈 로즈 플레이리스트 상세 화면 (ListWidget 기반)
        RoseHomeHisotryDetail *sub_historyDetail = nullptr;                 ///< 로즈홈 히스토리 상세 화면 (ListWidget 기반)
        RoseHomePlaylistListAll *sub_playlistAll = nullptr;                 ///< 로즈홈 플레이리스트 목록 전체 (flowLayout 기반)
        RoseHomeTrackListAll *sub_tracklistAll = nullptr;                   ///< 로즈홈 트렉 목록 전채 (ListWidget 기반)
        RoseHomeTrackListAll_Share *sub_tracklistAll_share = nullptr;       ///< 로즈홈 공유 트렉 목록 전채
        RoseHomeRosetubeListAll *sub_rosetubeAll = nullptr;                 ///< 로즈홈 로즈튜브 목록 전채 (flowLayout 기반)
        RoseHomePlaylistHistoryAll *sub_playlistHistoryAll = nullptr;       ///< 로즈홈 히스토리 플레이리스트 목록 전체
        RoseHomeAlbumHistoryAll *sub_albumHistoryAll = nullptr;             ///< 로즈홈 히스토리 앨범 목록 전체
        RoseHomeTrackHistoryAll *sub_trackHistroyAll = nullptr;             ///< 로즈홈 히스토리 트렉 목록 전체
        RoseHomeArtistHistoryAll *sub_artistHistroyAll = nullptr;           ///< 로즈홈 히스토리 아티스트 목록 전체
        RoseHomeHisotryListAll *sub_historyAll = nullptr;                   ///< 로즈홈 히스토리 목록 전체 (flowLayout 기반)
        RoseHomeAddPlaylist *sub_roseAddPlaylist = nullptr;                 ///< 로즈홈 플레이리스트 추가/삭제/변경 화면
        RoseHomeRecentlyListDelete *sub_roseRecentlyDelete = nullptr;       ///< 로즈홈 Recently 목록 삭제

        // 메뉴 관련 페이지  ---------------------- END
    };
};

#endif // ROSEHOMEMAIN_H
