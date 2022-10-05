#ifndef ITEMIMAGE_ALBUM_BUGS_H
#define ITEMIMAGE_ALBUM_BUGS_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"

namespace bugs {
    /**
     * @brief @sa ItemAlbum_bugs 에서 사용하는 벅스 Album용 Image파트 클래스
     */
    class ItemImage_Album_bugs : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Album_bugs(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_ALBUM_BUGS_H
