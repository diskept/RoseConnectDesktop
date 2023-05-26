#ifndef ROSEHOMEARTISTDETAIL_H
#define ROSEHOMEARTISTDETAIL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "tidal/TidalArtistDetail.h"
#include "bugs/BugsArtistDetail.h"
#include "qobuz/QobuzArtistDetail.h"
//#include "apple/AppleArtistDetail.h"

#include <QListWidget>


namespace roseHome {

    /**
     * @brief RoseHomeArtist 상세보기 화면
    */
    class RoseHomeArtistDetail : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeArtistDetail(QWidget *parent = nullptr);
        ~RoseHomeArtistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 섏씠吏 Show 붿껌  곗씠꾨떖諛쏅뒗 ⑸룄
        void setActivePage() override;

    private:
        // function about UI
        void setUIControl_basic();

    private:
        roseHome::ArtistItemData data_artist;

        QJsonObject artist_Obj;

        tidal::TidalArtistDetail *artistDetail_Tidal;
        bugs::BugsArtistDetail   *artistDetail_Bugs;
        qobuz::QobuzArtistDetail *artistDetail_Qobuz;
        //apple::AppleArtistDetail *artistDetail_Apple;
    };
};
#endif // ROSEHOMEARTISTDETAIL_H
