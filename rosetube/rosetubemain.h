#ifndef ROSETUBEMAIN_H
#define ROSETUBEMAIN_H

#include "home/abstractmaincontent.h"

#include "rosetube/rosetube_Home.h"
#include "rosetube/rosetube_Explore.h"
#include "rosetube/rosetube_category.h"
#include "rosetube/rosetube_viewall.h"
#include "rosetube/rosetube_viewalldetail.h"
#include "rosetube/rosetube_Subscription.h"
#include "rosetube/rosetube_Channel.h"
#include "rosetube/rosetube_Playlist.h"
#include "rosetube/rosetube_Collection.h"
#include "rosetube/rosetube_Search.h"

#include "rosetube/rosetube_ExploreGenre.h"
#include "rosetube/rosetube_ListViewAll.h"
#include "rosetube/rosetube_MyPlaylistAll.h"
#include "rosetube/rosetube_MyPlaylistDetail.h"
#include "rosetube/rosetube_MyFavorite.h"
#include "rosetube/rosetube_CacheViewAll.h"
#include "rosetube/rosetubeTrackListAll_Share.h"
#include "widget/myqwidget.h"//c220730


/**
 * @brief 로즈튜브 메인메뉴의 총괄 @n
 *      로즈튜브 대항목 관련 모든 QWidget 관리
 */
class RoseTubeMain : public AbstractMainContent
{
    Q_OBJECT
public:
    explicit RoseTubeMain(QWidget *parent = nullptr);

signals:

protected slots:
    void slot_overrideSigalSearch(bool b) override;//c220730
    void slot_dragEnterEvent_hide_show(bool show);//c220730

private slots:
    void slot_dragEnterEvent_restore();//c220826_1
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;//cheon211015

    void slot_changeRoseTubeBG(const QPixmap &p_pixmap);
    void slot_clickedViewAll(const QJsonObject &p_jsonData);
    void slot_search(const QString &p_text);
    void slot_totalsearch();//cheon211108

protected:
    void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

private :
    // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요
    QWidget *curr_widget;//c220730
    rosetube::RoseTubeHome *roseHome=nullptr;
    rosetube::RoseTubeExplore *roseExplore=nullptr;
    rosetube::RoseTubeExploreGenre *roseExploreGenre=nullptr;
    rosetube::RoseTube_Subscription *roseSubscription=nullptr;
    rosetube::RosetubeViewAll *roseViewAll=nullptr;
    rosetube::RoseTubeRecentlyPlaylistAll *roseRecentViewAll=nullptr;
    rosetube::RosetubeMyRosePlaylistAll *roseMyViewAll=nullptr;
    rosetube::RosetubeUserRosePlaylistAll *roseUserViewAll=nullptr;
    rosetube::RosetubeMyPlaylistDetail *roseMyPlaylistDetail=nullptr;
    rosetube::RoseTube_PlayList *rosePlayList=nullptr;
    rosetube::RoseTube_Channel *roseChannel=nullptr;
    rosetube::RoseTube_Collection *roseCollection=nullptr;
    rosetube::RoseTubeFavorite *roseFavorite=nullptr;
    rosetube::RoseTube_Search *roseSearch=nullptr;
    rosetube::RoseTubeCacheViewAll *roseCacheViewAll=nullptr;
    rosetube::RoseTubeTrackListAll_Share *roseShareViewAll=nullptr;
    CategorySelector *viewAll_category=nullptr;
    RoseTube_ViewAll *viewAll_playList=nullptr;
    RoseTube_ViewAllDetail *viewAll_detail=nullptr;

    void setInit();
    void setUIControl();
    void clearRoseTubeBG();

};

#endif // ROSETUBEMAIN_H
