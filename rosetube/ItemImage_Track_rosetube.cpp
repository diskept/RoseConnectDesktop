#include "rosetube/ItemImage_Track_rosetube.h"

namespace rosetube {
    /**
     * @brief ItemAlbum 에서 Image 영역에 대한 Widget
     */
    ItemImage_Track_rosetube::ItemImage_Track_rosetube(int imageWidth, int imageHeight, bool flagPlay, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageWidth;
        this->fixed_imgHeight = imageHeight;
        this->fixed_cornerRadius = 3;

        this->setUIControl();
        this->setImage("");                 // 기본 이미지 보이도록

        if(flagPlay){
            this->setUseHover(flagPlay);
            this->setUseBtn_favoriteRose(true, 0);
        }
        else{
            this->setUseHoverRose(true);
            this->setUseBtn_favorite(flagPlay);
        }
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Track_rosetube::getImagePath_default(){
        return ":/images/def_tube_340.png";
    }
}
