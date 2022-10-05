#include "roseRadio/ItemImage_Rectangle.h"

namespace roseRadio {
    /**
     * @brief ItemAlbum 에서 Image 영역에 대한 Widget
     */
    ItemImage_Rectangle::ItemImage_Rectangle(int imageSize, bool flagPlay, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageSize;
        this->fixed_imgHeight = imageSize;
        this->fixed_cornerRadius = 3;

        this->setUIControl();
        this->setImage("");                 // 기본 이미지 보이도록

        this->setUseBtn_favorite(false);

        if(flagPlay){
            this->setUseHover(flagPlay);
        }
        else{
            this->setUseHover2(flagPlay);
        }
    }

    ItemImage_Rectangle::~ItemImage_Rectangle(){

        this->deleteLater();
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Rectangle::getImagePath_default(){

        return ":/images/def_mus_300.png";
    }
}
