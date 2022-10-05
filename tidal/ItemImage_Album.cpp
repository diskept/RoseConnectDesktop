#include "ItemImage_Album.h"

namespace tidal {

    /**
     * @brief ItemAlbum 에서 Image 영역에 대한 Widget
     */
    ItemImage_Album::ItemImage_Album(int imageSize, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageSize;
        this->fixed_imgHeight = imageSize;
        this->fixed_cornerRadius = 3;

        this->setUIControl();
        this->setImage("");                 // 기본 이미지 보이도록

        this->setUseBtn_favorite(false);
        this->setUseHover(true);
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Album::getImagePath_default(){
        //return ":images/def_mus_200.png";
        return ":images/tidal/tidal_def_200.png";
    }

}
