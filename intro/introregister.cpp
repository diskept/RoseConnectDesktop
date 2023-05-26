#include "introregister.h"

#include "common/findip.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "widget/clickablewidget.h"
#include "widget/VerticalScrollArea.h"
#include "widget/searchdevicewidget.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QtMath>
#include <QRegExpValidator>
#include <widget/toastmsg.h>

const int MIN_HALF_W = 960;
IntroRegister::IntroRegister(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();

}

IntroRegister::~IntroRegister(){

}

/**
 * @brief IntroRegister::setInit : 초기 세팅
 */
void IntroRegister::setInit(){

    this->setStyleSheet("background-color:#212121;");
}

/**
 * @brief IntroRegister::setUIControl : UI 세팅
 */
void IntroRegister::setUIControl(){

    // 좌측 배경 이미지
    lb_img = new QLabel(this);
    lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/intro_img3.png"));
    lb_img->setMinimumSize(QSize(MIN_HALF_W,1080));
    lb_img->setScaledContents(true);

    QVBoxLayout *box_left = new QVBoxLayout();
    box_left->addWidget(lb_img);
    box_left->setSpacing(0);
    box_left->setMargin(0);
    box_left->setContentsMargins(0,0,0,0);

    QWidget *wg_bg = new QWidget(this);
    wg_bg->setLayout(box_left);
    wg_bg->setStyleSheet("background-color:#050101");


    // 우측 기기 선택 및 완료
    QLabel *lb_welcome = new QLabel(this);
    lb_welcome->setText(tr("Register ROSE"));
    lb_welcome->setStyleSheet("font-size:40px;color:#FFFFFF;");

    QLabel *lb_guide_1 = new QLabel(this);
    lb_guide_1->setText(tr("Pease select ROSE."));
    lb_guide_1->setStyleSheet("font-size:24px;color:#B3B3B3;margin-top:55px;margin-bottom:55px;");

    // Search
    wg_search = new SearchDeviceWidget(this);


    // 완료 버튼
    QPushButton *btn_next = new QPushButton(this);
    btn_next->setText(tr("Complete"));
    btn_next->setStyleSheet("font-size:18px;color:#FFFFFF;border-radius:20px;background-color:#B18658;");
    btn_next->setFixedSize(210,40);
    btn_next->setCursor(Qt::PointingHandCursor);





    QVBoxLayout *box_right = new QVBoxLayout();
    box_right->addStretch(5);
    box_right->addWidget(lb_welcome,0, Qt::AlignHCenter);
    box_right->addWidget(lb_guide_1,0, Qt::AlignHCenter);
    box_right->addWidget(wg_search,0, Qt::AlignHCenter);
    box_right->addStretch(3);
    box_right->addWidget(btn_next,0, Qt::AlignHCenter);
    box_right->addStretch(3);


    QHBoxLayout *boxLayout = new QHBoxLayout();
    boxLayout->addWidget(wg_bg,1, Qt::AlignTop);
    boxLayout->setSpacing(0);
    boxLayout->setMargin(0);
    boxLayout->setContentsMargins(0,0,0,0);




    this->setLayout(boxLayout);

    wg_right = new QWidget(this);
    wg_right->setLayout(box_right);

    QLabel *lb_icon = new QLabel(wg_bg);
    lb_icon->setPixmap(*GSCommon::getUIPixmapImg(":/images/intro_logo.png"));
    lb_icon->setGeometry(40,30,50,86);
    lb_icon->setStyleSheet("background:transparent;");

    connect(btn_next, SIGNAL(clicked()),  SLOT(complete()));

}





/**
 * @brief IntroRegister::resizeEvent : resize 이벤트
 * @details Widget의 사이즈 변경
 * @param event
 */
void IntroRegister::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    int width = this->width()/2;
    int height = this->height();

    double aspectRadio = (double)height/width;

    if(width >MIN_HALF_W || height > 1080){
        if(aspectRadio <=1.125){
            lb_img->setFixedWidth(width);
            lb_img->setFixedHeight(ceil(width*1.125));

            wg_right->setGeometry(width,0,this->width()-width, this->height());
        }else{
            int fixedHeight = ceil(height/1.125);
            lb_img->setFixedHeight(height);
            lb_img->setFixedWidth(fixedHeight);

            if(width > MIN_HALF_W){
                if(this->width()-fixedHeight >MIN_HALF_W){
                    wg_right->setGeometry(fixedHeight,0,this->width()-fixedHeight,this->height());
                }else{
                    wg_right->setGeometry(this->width()-MIN_HALF_W,0,MIN_HALF_W,this->height());
                }
            }else{
                wg_right->setGeometry(this->width()-MIN_HALF_W,0,MIN_HALF_W,this->height());

            }
        }
    }else{
        lb_img->setFixedWidth(MIN_HALF_W);
        lb_img->setFixedHeight(1080);

        wg_right->setGeometry(this->width()-MIN_HALF_W, 0, MIN_HALF_W, this->height());
    }
}

/**
 * @brief IntroRegister::searchAllDevice : 로즈 전체 기기 검색
 */
void IntroRegister::searchAllDevice(){
    if(wg_search != nullptr){
        print_debug();
        wg_search->searchDevice("");
    }
}


/**
 * @brief IntroRegister::complete : [슬롯] 완료버튼 클릭
 * @details 선택한 기기의 정보를 저장한다.
 */
void IntroRegister::complete(){

    if(wg_search->getFlagSelected()){
        if(wg_search->saveCurrDeviceData()){
            emit clickedNext();
        }else{
            ToastMsg::show(this,"",tr("ROSE device connection failed."));
        }
    }else{

        ToastMsg::show(this,"",tr("Please select a ROSE device."));
    }

}

/**
 * @brief IntroRegister::saveDeviceInfoDB : 현재 선택된 로즈기기 정보 DB 저장
 */
void IntroRegister::saveDeviceInfoDB(){
    wg_search->saveCurrDeviceData();
}
