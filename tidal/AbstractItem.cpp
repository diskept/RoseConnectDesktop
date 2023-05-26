#include "AbstractItem.h"

namespace tidal {

    /**
     * @brief AbstractItem::AbstractItem
     * @param parent
     */
    AbstractItem::AbstractItem(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent) : QWidget(parent) {
        this->m_index = index;
        this->m_section = section;
        this->m_imageSizeMode = imageSizeMode;
    }


    /**
     * @brief index 반환 (생성자로 부터 받았던 index 값임. 변경없이 그대로 전달한다.)
     * @return
     */
    int AbstractItem::index(){
        return this->m_index;
    }


    /**
     * @brief section 반환 (생성자로 부터 받았던 index 값임. 변경없이 그대로 전달한다.)
     * @return
     */
    int AbstractItem::section(){
        return this->m_section;
    }


    /**
     * @brief AbstractItem::get_imageWidth
     * @param imageSizeMode
     * @return
     */
    int AbstractItem::get_imageWidth(ImageSizeMode& imageSizeMode){
        switch (imageSizeMode) {
            case ImageSizeMode::Square_120x120: return 120;
            case ImageSizeMode::Square_180x180: return 180;
            case ImageSizeMode::Square_186x186: return 186;
            case ImageSizeMode::Square_200x200: return 200;
            case ImageSizeMode::Square_284x284: return 284;
            case ImageSizeMode::Ractangle_200x281: return 200;
            case ImageSizeMode::Ractangle_251x139: return 251;
            case ImageSizeMode::Ractangle_284x157: return 284;
            case ImageSizeMode::Ractangle_340x188: return 340;
            case ImageSizeMode::Ractangle_358x174: return 358;
            case ImageSizeMode::Ractangle_360x176: return 360;
            case ImageSizeMode::Ractangle_360x200: return 360;
            case ImageSizeMode::Ractangle_384x186: return 384;
            case ImageSizeMode::Ractangle_450x120: return 450;
            case ImageSizeMode::Ractangle_480x120: return 480;

            default: return 200;             // windows msmv2007 compiler warning 때문에 넣음 (이 경우는 없음)  by sunnyfish
        }
    }

    /**
     * @brief AbstractItem::get_imageHeight
     * @param imageSizeMode
     * @return
     */
    int AbstractItem::get_imageHeight(ImageSizeMode& imageSizeMode){
        switch (imageSizeMode) {
            case ImageSizeMode::Square_120x120: return 120;
            case ImageSizeMode::Square_180x180: return 180;
            case ImageSizeMode::Square_186x186: return 186;
            case ImageSizeMode::Square_200x200: return 200;
            case ImageSizeMode::Square_284x284: return 284;
            case ImageSizeMode::Ractangle_200x281: return 281;
            case ImageSizeMode::Ractangle_251x139: return 139;
            case ImageSizeMode::Ractangle_284x157: return 157;
            case ImageSizeMode::Ractangle_340x188: return 188;
            case ImageSizeMode::Ractangle_358x174: return 174;
            case ImageSizeMode::Ractangle_360x176: return 176;
            case ImageSizeMode::Ractangle_360x200: return 200;
            case ImageSizeMode::Ractangle_384x186: return 186;
            case ImageSizeMode::Ractangle_450x120: return 120;
            case ImageSizeMode::Ractangle_480x120: return 120;

            default:    return 200;         // windows msmv2007 compiler warning 때문에 넣음 (이 경우는 없음)  by sunnyfish
        }
    }


    /**
     * @brief AbstractItem::get_spaceImageAndTitle
     * @param imageSizeMode
     * @return
     */
    int AbstractItem::get_spaceImageAndTitle(ImageSizeMode& imageSizeMode){
        switch (imageSizeMode) {
            case ImageSizeMode::Square_120x120: return 10;
            case ImageSizeMode::Square_180x180: return 10;
            case ImageSizeMode::Square_186x186: return 10;
            case ImageSizeMode::Square_200x200: return 15;
            case ImageSizeMode::Square_284x284: return 20;
            case ImageSizeMode::Ractangle_200x281: return 15;
            case ImageSizeMode::Ractangle_251x139: return 15;
            case ImageSizeMode::Ractangle_284x157: return 15;
            case ImageSizeMode::Ractangle_340x188: return 10;
            case ImageSizeMode::Ractangle_358x174: return 15;
            case ImageSizeMode::Ractangle_360x176: return 15;
            case ImageSizeMode::Ractangle_360x200: return 15;
            case ImageSizeMode::Ractangle_384x186: return 15;
            case ImageSizeMode::Ractangle_450x120: return 10;
            case ImageSizeMode::Ractangle_480x120: return 10;

            default: return 10;     // windows msmv2007 compiler warning 때문에 넣음 (이 경우는 없음) by sunnyfish
        }
    }



    /**
     * @brief ItemAlbum::slot_itemImage_clickedBtn
     * @param clickMode
     */
    void AbstractItem::slot_itemImage_clickedBtn(AbstractItemImage::BtnClickMode clickMode){
        if(clickMode == AbstractItemImage::BtnClickMode::AllBox){
            emit this->signal_clicked(ClickMode::AllBox);
        }
        else if(clickMode == AbstractItemImage::BtnClickMode::Play_RightNow){
            emit this->signal_clicked(ClickMode::PlayBtn);
        }
        else if(clickMode == AbstractItemImage::BtnClickMode::More_showPopupMenu){
            emit this->signal_clicked(ClickMode::MoreBtn);
        }
        else if(clickMode == AbstractItemImage::BtnClickMode::Favorite_toAdd){
            emit this->signal_clicked(ClickMode::FavBtn_Add);
        }
        else if(clickMode == AbstractItemImage::BtnClickMode::Favorite_toAddx2){
            emit this->signal_clicked(ClickMode::FavBtn_Addx2);
        }
        else if(clickMode == AbstractItemImage::BtnClickMode::Favorite_toAddx3){
            emit this->signal_clicked(ClickMode::FavBtn_Addx3);
        }
        else if(clickMode == AbstractItemImage::BtnClickMode::Favorite_toDelete){
            emit this->signal_clicked(ClickMode::FavBtn_Delete);
        }
    }

}
