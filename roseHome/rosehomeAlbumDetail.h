#ifndef ROSEHOMEALBUMDETAIL_H
#define ROSEHOMEALBUMDETAIL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"
#include "roseHome/rosehomeAlbumDetail_Rose.h"

#include "tidal/TidalAlbumDetail.h"
#include "bugs/BugsAlbumDetail.h"
#include "qobuz/QobuzAlbumDetail.h"
#include "apple/AppleAlbumDetail.h"

#include <QListWidget>


namespace roseHome {

    /**
     * @brief RoseHomeAlbum 상세보기 화면
     */
    class RoseHomeAlbumDetail : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeAlbumDetail(QWidget *parent = nullptr);
        ~RoseHomeAlbumDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 섏씠吏 Show 붿껌  곗씠꾨떖諛쏅뒗 ⑸룄
        void setActivePage() override;

    private:
        // function about UI
        void setUIControl_basic();

    private:
        roseHome::AlbumItemData data_album;

        QJsonObject album_Obj;

        roseHome::RoseHomeAlbumDetail_Rose *albumDetail_Rose;
        tidal::TidalAlbumDetail *albumDetail_Tidal;
        bugs::BugsAlbumDetail   *albumDetail_Bugs;
        qobuz::QobuzAlbumDetail *albumDetail_Qobuz;
        apple::AppleAlbumDetail *albumDetail_Apple;
    };
};
#endif // ROSEHOMEALBUMDETAIL_H
