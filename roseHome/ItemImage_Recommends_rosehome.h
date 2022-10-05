#ifndef ITEMIMAGE_RECOMMENDS_ROSEHOME_H
#define ITEMIMAGE_RECOMMENDS_ROSEHOME_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"


namespace roseHome {
    /**
     * @brief @sa ItemRecommends 에서 사용하는 ROSEHOME Recommends용 Image파트 클래스
     */
    class ItemImage_Recommends_roseHome : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Recommends_roseHome(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};
#endif // ITEMIMAGE_RECOMMENDS_ROSEHOME_H
