#include "ItemImage_Video_bugs.h"
#include <QGraphicsOpacityEffect>

namespace bugs {

    /**
     * @brief ItemVideo 에서 Image 영역에 대한 Widget
     */
    ItemImage_Video_bugs::ItemImage_Video_bugs(int imageWidth, int imageHeight, QWidget *parent)
        : tidal::AbstractItemImage(parent) {

        this->fixed_imgWidth = imageWidth;
        this->fixed_imgHeight = imageHeight;
        this->fixed_cornerRadius = 3;

        this->setUIControl();
        this->setImage("");                 // 기본 이미지 보이도록
        this->setUIControl_duration();                  // 추가적인 UI 세팅

        this->setUseBtn_favorite(false);
        this->setUseHover(true);
    }


    /**
     * @brief 기본 이미지 반환
     * @return
     */
    QString ItemImage_Video_bugs::getImagePath_default(){
        //return ":images/def_tube_340.png";
        return ":/images/bugs/bugs_def_284.png";
    }


    /**
     * @brief Duration 정보를 표현하는 Label에 대한 UI 세팅
     */
    void ItemImage_Video_bugs::setUIControl_duration(){

        const int WIDTH_DURATION = 80;
        const int HEIGHT_DURATION = 30;

        // UI for duration BG box
        this->label_durationBG = new QLabel(this->label_imageBig);
        this->label_durationBG->setFixedSize(WIDTH_DURATION, HEIGHT_DURATION);
        this->label_durationBG->setStyleSheet( QString("background-color:#000000; border-bottom-left-radius:%1px;").arg(this->fixed_cornerRadius) );
        QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
        eff->setOpacity(0.6);
        this->label_durationBG->setGraphicsEffect(eff);
        this->label_durationBG->setVisible(false);        // 기본값
        this->label_durationBG->move(0, this->fixed_imgHeight-HEIGHT_DURATION);

        // UI for duration text
        this->label_durationText = new QLabel(this->label_imageBig);
        this->label_durationText->setFixedSize(WIDTH_DURATION, HEIGHT_DURATION);
        this->label_durationText->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        this->label_durationText->setStyleSheet("background-color:transparent; color:#ffffff; font-size:18px;");
        this->label_durationText->setVisible(false);        // 기본값
        this->label_durationText->move(0, this->fixed_imgHeight-HEIGHT_DURATION);

    }


    /**
     * @brief duration 텍스트 표기
     * @param len
     */
    void ItemImage_Video_bugs::setDuration(const QString len){
        if(len.isEmpty()){
            this->label_durationBG->setVisible(false);
            this->label_durationText->setVisible(false);
        }
        else{
            this->label_durationBG->setVisible(true);
            this->label_durationText->setVisible(true);
            this->label_durationText->setText(len);
        }
    }

}
