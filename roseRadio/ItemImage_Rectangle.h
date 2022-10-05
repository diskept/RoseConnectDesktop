#ifndef ITEMIMAGE_RECTANGLE_H
#define ITEMIMAGE_RECTANGLE_H

#include "tidal/AbstractItemImage.h"

#include <QWidget>


namespace roseRadio {

    /**
     * @brief @sa ItemRecommends 먯꽌 ъ슜섎뒗 ROSEHOME RecommendsImage뚰듃 대옒     */
    class ItemImage_Rectangle : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Rectangle(int imageSize, bool flagPlay, QWidget *parent = nullptr);
        ~ItemImage_Rectangle();

    protected:
        QString getImagePath_default() override;

    };
};
#endif // ITEMIMAGE_RECTANGLE_H
