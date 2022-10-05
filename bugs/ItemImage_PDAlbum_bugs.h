#ifndef ITEMIMAGE_PDALBUM_BUGS_H
#define ITEMIMAGE_PDALBUM_BUGS_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"

namespace bugs {
    /**
     * @brief @sa ItemPD_Album_bugs 에서 사용하는 벅스 PD's Album용 Image파트 클래스
     */
    class ItemImage_PDAlbum_bugs : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_PDAlbum_bugs(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};


#endif // ITEMIMAGE_PDALBUM_BUGS_H
