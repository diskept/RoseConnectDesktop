#include "ItemImage_Playlist_forQobuz.h"

namespace qobuz {

    /**
     * @brief ItemPlaylist 에서 Image 영역에 대한 Widget
     */
    ItemImage_Playlist_qobuz::ItemImage_Playlist_qobuz(int imageWidth, int imageHeight, QWidget *parent)
        : AbstractItemImage(parent) {

        this->fixed_imgWidth = imageWidth;
        this->fixed_imgHeight = imageHeight;
        this->fixed_cornerRadius = 3;

        //qDebug() << "+++++Qobuz ItemImage_Playlist_forQobuz.cpp--ItemImage_Playlist_qobuz()----------------------------------";
        //qDebug() << "ItemPlaylist width :" << this->fixed_imgWidth << "ItemPlaylist height :" << this->fixed_imgHeight;

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
    QString ItemImage_Playlist_qobuz::getImagePath_default(){
        //return ":images/def_mus_200.png";
        return ":/images/qobuz/qobuz_default_384.png";
    }

}
