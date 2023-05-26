#ifndef ITEMIMAGE_ARTIST_FORAPPLE_H
#define ITEMIMAGE_ARTIST_FORAPPLE_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"


namespace apple {
    /**
     * @brief @sa ItemArtist 에서 사용하는 Qobuz Artist용 Image파트 클래스
     */
    class ItemImage_Artist_apple : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Artist_apple(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};
#endif // ITEMIMAGE_ARTIST_FORAPPLE_H
