#ifndef ITEMIMAGE_WIDGET_VIDEO_H
#define ITEMIMAGE_WIDGET_VIDEO_H

#include "tidal/AbstractItemImage.h"
#include <QWidget>


namespace video {

    /**
     * @brief @sa ItemRecommends 에서 사용하는 ROSEHOME Recommends용 Image파트 클래스
     */
    class ItemImage_Widget_video : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Widget_video(int imageSize_w, int imageSize_h, bool flagPlay, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;
    };
};

#endif // ITEMIMAGE_WIDGET_VIDEO_H
