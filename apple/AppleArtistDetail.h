#ifndef APPLEARTISTDETAIL_H
#define APPLEARTISTDETAIL_H

#include "apple/AbstractAppleSubWidget.h"

//#include "qobuz/ItemAlbum_forQobuz.h"
//#include "qobuz/ItemArtist_forQobuz.h"

#include "widget/AbstractImageDetailContents_RHV.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/dialogNotice.h"


namespace apple {

    /**
     * @brief Qobuz > Artist 상세화면 클래스
     */
    class AppleArtistDetail : public AbstractAppleSubWidget
    {
        Q_OBJECT
    public:
        explicit AppleArtistDetail(QWidget *parent = nullptr);
        ~AppleArtistDetail();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;
    };
};
#endif // APPLEARTISTDETAIL_H
