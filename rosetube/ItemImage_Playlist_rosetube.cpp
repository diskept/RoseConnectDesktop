#include "rosetube/ItemImage_Playlist_rosetube.h"

namespace rosetube {
    /**
     * @brief ItemAlbum 에서 Image 영역에 대한 Widget
     */
    ItemImage_Playlist_rosetube::ItemImage_Playlist_rosetube(int imageWidth, int imageHeight, bool flagPlay, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageWidth;
        this->fixed_imgHeight = imageHeight;
        this->fixed_cornerRadius = 3;

        this->setUIControl();
        this->show_topListBG(true);         // 여러겹 표현
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
    QString ItemImage_Playlist_rosetube::getImagePath_default(){
        return ":/images/def_tube_340.png";
    }
}
