#include "ItemImage_Playlist_rosehome.h"

namespace roseHome {
    /**
     * @brief ItemPlaylist 에서 Image 영역에 대한 Widget
     */
    ItemImage_Playlist_roseHome::ItemImage_Playlist_roseHome(int imageWidth, int imageHeight, bool flagPlay, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageWidth;
        this->fixed_imgHeight = imageHeight;
        this->fixed_cornerRadius = 3;

        this->setUIControl();
        this->show_topListBG(true);         // 여러겹 표현
        this->setImage("");                 // 기본 이미지 보이도록

        this->setUseBtn_favorite(false);
        this->setUseHover2(flagPlay);
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Playlist_roseHome::getImagePath_default(){
        return ":images/def_mus_300.png";
    }
}
