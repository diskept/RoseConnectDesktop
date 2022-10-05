#include "ItemImage_Artist.h"

namespace tidal {

    /**
     * @brief ItemArtist 에서 Image 영역에 대한 Widget
     */
    ItemImage_Artist::ItemImage_Artist(int imageSize, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageSize;
        this->fixed_imgHeight = imageSize;
        this->fixed_cornerRadius = this->fixed_imgWidth/2;      // 원형 처리

        this->setUIControl();
        this->setImage("");                 // 기본 이미지 보이도록

        this->setUseBtn_favorite(false);
        this->setUseBtn_play(false);
        this->setUseBtn_more(true);

        // hover 만 사용
        this->setUseHover(true);
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Artist::getImagePath_default(){
        //return ":images/def_mus_200.png";
        //return ":/images/tidal/artist_def_200.png";
        return ":/images/rosehome/artist_def.png";
    }

}
