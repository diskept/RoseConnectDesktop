#ifndef ROSEHOMEPLAYLISTDETAIL_H
#define ROSEHOMEPLAYLISTDETAIL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"
#include "roseHome/rosehomePlaylistDetail_Rose.h"

#include "music/musice_playlist.h"
#include "tidal/TidalPlaylistDetail.h"
#include "tidal/TidalMixesDetail.h"
#include "bugs/BugsPDAlbumDetail.h"
#include "qobuz/QobuzPlaylistDetail.h"
#include "apple/ApplePlaylistDetail.h"

#include <QListWidget>


namespace roseHome {

    /**
     * @brief RoseHome의 Album 상세화면 클래스
     */
    class RoseHomePlaylistDetail : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomePlaylistDetail(QWidget *parent = nullptr);
        ~RoseHomePlaylistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;

    private:
        // function about UI
        void setUIControl_basic();

    private:
        roseHome::PlaylistItemData data_playlist;

        QJsonObject playlist_Obj;
        QString tidal_mix_chk = "";

        roseHome::RoseHomePlaylistDetail_Rose *playlistDetail_Rose;
        Musice_playlist             *playlistDetail_music;
        tidal::TidalPlaylistDetail  *playlistDetail_Tidal;
        tidal::TidalMixesDetail     *mixlistDetail_Tidal;
        bugs::BugsPDAlbumDetail     *pdDlbumDetail_Bugs;
        qobuz::QobuzPlaylistDetail  *playlistDetail_Qobuz;
        apple::ApplePlaylistDetail  *playlistDetail_Apple;
    };
};
#endif // ROSEHOMEPLAYLISTDETAIL_H
