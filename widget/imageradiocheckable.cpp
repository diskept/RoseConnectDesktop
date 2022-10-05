#include "imageradiocheckable.h"

ImageRadioCheckable::ImageRadioCheckable(QWidget *parent, Mode mode)
    : ImageLabelRadio(parent, mode)
{
    setCheckUIControl();
}


/**
 * @brief ImageRadioCheckable::setUIControl : UI 세팅
 */
void ImageRadioCheckable::setCheckUIControl(){

    wgBGSelected = new QWidget(lb_img);
    wgBGSelected->setObjectName("wgCheck");
    wgBGSelected->setFixedSize(IMG_WIDTH, IMG_HEIGHT);
    wgBGSelected->setStyleSheet("#wgCheck{background-color:#B3000000;border-radius:3px;border:4px solid #B18658;}");
    wgBGSelected->setAutoFillBackground(true);
    wgBGSelected->hide();


    wg_hover->raise();

    checkBox = new QCheckBox(lb_img);
    checkBox->setObjectName("checkBox");
    checkBox->setStyleSheet(
                "#checkBox {background-color:transparent;font-size:18px;color:#E6E6E6;spacing:0px;}"
                "QCheckBox::indicator:unchecked { image: url(:images/list_check_off.png);}"
                "QCheckBox::indicator:checked { image: url(:images/list_check_on.png);}"
                );
    checkBox->setCursor(Qt::PointingHandCursor);
    checkBox->setFixedHeight(50);

    connect(checkBox, SIGNAL(stateChanged(int)), SLOT(slot_checkedChanged(int)));

}

void ImageRadioCheckable::setCheckedUI(){
    checkBox->setChecked(true);
    wgBGSelected->show();
    wg_hover->setStyleSheet("#hoverWidget{background-color:#B3000000;border-radius:3px;border:4px solid #B18658;}");
}

void ImageRadioCheckable::setUnCheckedUI(){
    checkBox->setChecked(false);
    wgBGSelected->hide();
    wg_hover->setStyleSheet("#hoverWidget{background-color:#B3000000;border-radius:3px;}");
}

void ImageRadioCheckable::checked(bool p_flagCheck){
    flagIsChecked = p_flagCheck;

    if(flagIsChecked){
        setCheckedUI();
    }else{
        setUnCheckedUI();
    }
}

bool ImageRadioCheckable::getCheckedValue(){
    return flagIsChecked;
}

/**
 * @brief ImageLabel::enterEvent : 마우스 Enter 이벤트 슬롯
 * @param event QEvent
 */
void ImageRadioCheckable::enterEvent(QEvent *event){

    ImageLabelRadio::enterEvent(event);

    // hover alpha 배경과 중첩=> 선택상태 배경 hide 처리
    if(flagIsChecked){
        wgBGSelected->hide();
    }
}

/**
 * @brief ImageLabel::leaveEvent : 마우스 Leave 이벤트 슬롯
 * @param event QEvent
 */
void ImageRadioCheckable::leaveEvent(QEvent *event){

    ImageLabelRadio::leaveEvent(event);

    if(flagIsChecked){
        wgBGSelected->show();
    }
}

void ImageRadioCheckable::slot_checkedChanged(int p_value){
    flagIsChecked = p_value > 0 ? true : false;

    if(flagIsChecked){
        setCheckedUI();
    }else{
        setUnCheckedUI();
    }
}
