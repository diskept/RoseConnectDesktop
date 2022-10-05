#include "video/ItemImage_Widget_video.h"


namespace video {
    /**
     * @brief ItemAlbum 에서 Image 영역에 대한 Widget
     */
    ItemImage_Widget_video::ItemImage_Widget_video(int imageSize_w, int imageSize_h, bool flagPlay, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageSize_w;
        this->fixed_imgHeight = imageSize_h;
        this->fixed_cornerRadius = 3;

        this->setUIControl();
        this->setImage("");                 // 기본 이미지 보이도록

        this->setUseHover(flagPlay);
        this->setUseBtn_favorite(false);
        this->setUseBtn_more(false);
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Widget_video::getImagePath_default(){
        return ":/images/def_video_200.png";
    }
}
