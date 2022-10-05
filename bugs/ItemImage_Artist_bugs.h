#ifndef ITEMIMAGE_ARTIST_BUGS_H
#define ITEMIMAGE_ARTIST_BUGS_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"

namespace bugs {
    /**
     * @brief @sa ItemArtist_bugs 에서 사용하는 벅스 Artist용 Image파트 클래스
     */
    class ItemImage_Artist_bugs : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Artist_bugs(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};


#endif // ITEMIMAGE_ARTIST_BUGS_H
