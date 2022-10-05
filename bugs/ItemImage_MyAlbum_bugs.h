#ifndef ITEMIMAGE_MYALBUM_BUGS_H
#define ITEMIMAGE_MYALBUM_BUGS_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"

namespace bugs {
    /**
     * @brief @sa ItemMyAlbum_bugs 에서 사용하는 벅스 MyAlbum용 Image파트 클래스
     */
    class ItemImage_MyAlbum_bugs : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_MyAlbum_bugs(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};


#endif // ITEMIMAGE_MYALBUM_BUGS_H
