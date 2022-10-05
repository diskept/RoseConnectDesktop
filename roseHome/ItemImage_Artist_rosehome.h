#ifndef ITEMIMAGE_ARTIST_ROSEHOME_H
#define ITEMIMAGE_ARTIST_ROSEHOME_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"


namespace roseHome {
    /**
     * @brief @sa ItemArtist 에서 사용하는 Qobuz Artist용 Image파트 클래스
     */
    class ItemImage_Artist_roseHome : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Artist_roseHome(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_ARTIST_ROSEHOME_H
