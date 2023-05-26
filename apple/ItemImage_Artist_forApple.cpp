#include "apple/ItemImage_Artist_forApple.h"


namespace apple {

    /**
     * @brief ItemArtist 에서 Image 영역에 대한 Widget
     */
    ItemImage_Artist_apple::ItemImage_Artist_apple(int imageSize, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageSize;
        this->fixed_imgHeight = imageSize;
        this->fixed_cornerRadius = this->fixed_imgWidth / 2;      // 원형 처리

        this->setUIControl();
        this->setImage("");                 // 기본 이미지 보이도록

        this->setUseBtn_favorite(false);
        this->setUseBtn_play(false);
        this->setUseBtn_more(true);
        this->setUseArtist(true);

        // hover 만 사용
        this->setUseHover(true);
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Artist_apple::getImagePath_default(){

        return ":/images/rosehome/artist_def.png";
    }

}
