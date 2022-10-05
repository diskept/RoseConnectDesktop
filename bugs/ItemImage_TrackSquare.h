#ifndef ITEMIMAGE_TRACKSQUARE_H
#define ITEMIMAGE_TRACKSQUARE_H

#include "tidal/AbstractItemImage.h"


namespace bugs {
    /**
     * @brief @sa ItemTrackSquare_bugs 에서 사용하는 벅스 Track(Square)용 Image파트 클래스
     * @details ItemImage_Album 을 상속해서 해도 되는데, tidal, bugs 종속성을 없애려고. (차후 변경사항 있을수 있으니)
     */
    class ItemImage_TrackSquare : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_TrackSquare(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_TRACKSQUARE_H
