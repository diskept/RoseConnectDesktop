#ifndef ITEMIMAGE_PLAYLIST_ROSETUBE_H
#define ITEMIMAGE_PLAYLIST_ROSETUBE_H

#include "tidal/AbstractItemImage.h"

#include <QWidget>


namespace rosetube {
    /**
     * @brief @sa ItemRecommends 에서 사용하는 ROSEHOME Recommends용 Image파트 클래스
     */
    class ItemImage_Playlist_rosetube : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Playlist_rosetube(int imageWidth, int imageHeight, bool flagPlay, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_PLAYLIST_ROSETUBE_H
