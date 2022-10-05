#ifndef APPLEMAIN_H
#define APPLEMAIN_H

#include "apple/AppleHome.h"

#include "apple/AppleAlbumListAll_Rose.h"
#include "apple/ApplePlaylistListAll_Rose.h"
#include "apple/AppleTrackListAll_Rose.h"
#include "apple/AppleMyPlaylistListAll_Rose.h"
#include "apple/AppleUserPlaylistListAll_Rose.h"

#include "apple/AppleAlbumDetail.h"
#include "apple/ApplePlaylistDetail.h"
#include "apple/ApplePlaylistDetail_Rose.h"

#include "home/abstractmaincontent.h"

#include <QJsonObject>
#include <QWidget>


namespace apple {

    /**
     * @brief Apple Main 클래스. (서브 페이지들을 관할하는
     * @sa AbstractMainContent 자식 클래스)
     */
    class AppleMain : public AbstractMainContent
    {
        Q_OBJECT
    public:
        explicit AppleMain(QWidget *parent = nullptr);
        ~AppleMain();

    private slots:
        void slot_clickedMoveSubPage(const QJsonObject &p_jsonData);

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

    private :
        Linker *linker;

        // UI 세팅
        void setUIControl();

        // 데이터 반환
        QJsonObject getJsonObject_forMainTitle();
        QJsonArray getJsonArray_forSubTitles();

        // 페이지 이동 처리
        template<typename T_AppleMusicHomeSub>
        T_AppleMusicHomeSub* procCommon_showSubPage(const bool flagShow_topBar, T_AppleMusicHomeSub *sub_widget, const QJsonObject& jsonObj_data);

        // 메뉴 관련 페이지  ---------------------- START
        AppleHome *sub_home = nullptr;                                          ///< APPLE MUSIC > Home
        //RoseFavorite *sub_favorite = nullptr;                               ///< APPLE MUSIC > Favorite
        //RoseHomeUserPick *sub_userpick = nullptr;                           ///< APPLE MUSIC > User Pick
        //RoseHomeFriend *sub_friend = nullptr;                               ///< APPLE MUSIC > Friends

        // 메뉴 서브 페이지
        AppleAlbumListAll_Rose *sub_albumListAll_rose = nullptr;                ///< APPLE MUSIC > Home > Recently album list all
        ApplePlaylistListAll_Rose *sub_playlistListAll_rose = nullptr;          ///< APPLE MUSIC > Home > Recently Playlist list all
        AppleTrackListAll_Rose *sub_trackListAll_rose = nullptr;                ///< APPLE MUSIC > Home > Recently album list all
        AppleMyPlaylistListAll_Rose *sub_myplaylistListAll_rose = nullptr;      ///< APPLE MUSIC > Home > My Playlist list all
        AppleUserPlaylistListAll_Rose *sub_userplaylistListAll_rose = nullptr;  ///< APPLE MUSIC > Home > User Playlist list all

        // 공통
        AppleAlbumDetail *sub_albumDetail = nullptr;                            ///< APPLE MUSIC Album Detail
        ApplePlaylistDetail *sub_playlistDetail = nullptr;                      ///< APPLE MUSIC Playlist Detail
        ApplePlaylistDetail_Rose *sub_playlistDetail_rose = nullptr;            ///< APPLE MUSIC Rose Playlist Detail

        //RoseHomeAlbumListAll *sub_albumAll = nullptr;                       ///< APPLE MUSIC 앨범 목록 전채 (flowLayout 기반)
        //RoseHomePlaylistListAll *sub_playlistAll = nullptr;                 ///< APPLE MUSIC 플레이리스트 목록 전채 (flowLayout 기반)
        //RoseHomeTrackListAll *sub_tracklistAll = nullptr;                   ///< APPLE MUSIC 트렉 목록 전채 (ListWidget 기반)
        //RoseHomeRosetubeListAll *sub_rosetubeAll = nullptr;                 ///< APPLE MUSIC 로즈튜브 목록 전채 (flowLayout 기반)
        //RoseHomeAddPlaylist *sub_roseAddPlaylist = nullptr;                 ///< APPLE MUSIC 플레이리스트 추가/삭제/변경 화면

        // 메뉴 관련 페이지  ---------------------- END
    };
};
#endif // APPLEMAIN_H
