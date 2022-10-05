#include "introwifi.h"

#include "common/gscommon.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtMath>

const int MIN_HALF_W = 960;

/**
 * @brief IntroWifi::IntroWifi : 인트로 INTERNET
 * @details 인트로 두번째페이지 인터넷 연결하기
 * @param parent QWidget
 */
IntroWifi::IntroWifi(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

IntroWifi::~IntroWifi(){

}

/**
 * @brief IntroWifi::setInit : 초기세팅
 */
void IntroWifi::setInit(){
    this->setStyleSheet("background-color:#212121;");
}

/**
 * @brief IntroWifi::setUIControl : UI 세팅
 */
void IntroWifi::setUIControl(){

    lb_img = new QLabel(this);
    lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/intro_img2.png"));
    lb_img->setMinimumSize(QSize(MIN_HALF_W,1080));
    lb_img->setScaledContents(true);

    QVBoxLayout *box_left = new QVBoxLayout();
    box_left->addWidget(lb_img, Qt::AlignTop);
    box_left->setSpacing(0);
    box_left->setMargin(0);
    box_left->setContentsMargins(0,0,0,0);

    QWidget *wg_bg = new QWidget(this);
    wg_bg->setLayout(box_left);
    wg_bg->setStyleSheet("background-color:#050101");


    QLabel *lb_internet = new QLabel(this);
    lb_internet->setText(tr("Connecting to the Internet"));
    lb_internet->setStyleSheet("font-size:40px;color:#FFFFFF");

    QLabel *lb_guide_1 = new QLabel(this);
    lb_guide_1->setText(tr("To use ROSE products with ROSE Cutect"));
    lb_guide_1->setStyleSheet("font-size:24px;color:#B3B3B3;margin-top:40px;");

    QLabel *lb_guide_2 = new QLabel(this);
    lb_guide_2->setText(tr("Plaese connect to the same network."));
    lb_guide_2->setStyleSheet("font-size:24px;color:#B3B3B3");

    QLabel *lb_imgWifi = new QLabel(this);
    lb_imgWifi->setPixmap(*GSCommon::getUIPixmapImg(":/images/intro_wifi.png"));
    lb_imgWifi->setStyleSheet("margin-top:10px;margin-bottom:5px;");

    QPushButton *btn_next = new QPushButton(this);
    btn_next->setText(tr("next"));
    btn_next->setStyleSheet("font-size:18px;color:#FFFFFF;border-radius:20px;background-color:#B18658;");
    btn_next->setFixedSize(210,40);
    btn_next->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *box_right = new QVBoxLayout();
    box_right->addStretch(3);
    box_right->addWidget(lb_internet,0, Qt::AlignHCenter);
    box_right->addWidget(lb_guide_1,0, Qt::AlignHCenter);
    box_right->addWidget(lb_guide_2,0, Qt::AlignHCenter);
    box_right->addWidget(lb_imgWifi,0, Qt::AlignHCenter);
    box_right->addWidget(btn_next,0, Qt::AlignHCenter);
    box_right->addStretch(2);




    QHBoxLayout *boxLayout = new QHBoxLayout();
    boxLayout->addWidget(wg_bg,1);
    //boxLayout->addWidget(wg_right,1);
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

    connect(btn_next, SIGNAL(clicked()), this, SIGNAL(clickedNext()));
}

void IntroWifi::resizeEvent(QResizeEvent *event){
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
