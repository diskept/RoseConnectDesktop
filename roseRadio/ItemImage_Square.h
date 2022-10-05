#ifndef ITEMIMAGE_SQUARE_H
#define ITEMIMAGE_SQUARE_H

#include "tidal/AbstractItemImage.h"

#include <QWidget>


namespace roseRadio {

    /**
     * @brief @sa ItemRecommends 먯꽌 ъ슜섎뒗 ROSEHOME RecommendsImage뚰듃 대옒     */
    class ItemImage_Square : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Square(int imageSize, bool flagPlay, QWidget *parent = nullptr);
        ~ItemImage_Square();

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_SQUARE_H
