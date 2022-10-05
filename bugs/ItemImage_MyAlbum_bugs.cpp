#include "ItemImage_MyAlbum_bugs.h"


namespace bugs {

    /**
     * @brief ItemAlbum 에서 Image 영역에 대한 Widget
     */
    ItemImage_MyAlbum_bugs::ItemImage_MyAlbum_bugs(int imageSize, QWidget *parent)
        : tidal::AbstractItemImage(parent) {

        this->fixed_imgWidth = imageSize;
        this->fixed_imgHeight = imageSize;
        this->fixed_cornerRadius = 3;

        this->setUIControl();
        this->show_topListBG(true);             // 여러겹 표현
        this->setImage("");                 // 기본 이미지 보이도록

        this->setUseBtn_favorite(false);
        this->setUseHover(true);
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_MyAlbum_bugs::getImagePath_default(){
        return ":images/playlist-def-big.png";                  // 기본 이미지가 다르다.
    }

}
