#include "roseHome/ItemImage_Album_rosehome.h"

namespace roseHome {
    /**
     * @brief ItemAlbum 에서 Image 영역에 대한 Widget
     */
    ItemImage_Album_roseHome::ItemImage_Album_roseHome(int imageSize, bool flagPlay, QWidget *parent)
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


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Album_roseHome::getImagePath_default(){
        return ":/images/def_mus_300.png";
    }
}
