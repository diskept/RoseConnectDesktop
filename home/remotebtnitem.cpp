#include "common/gscommon.h"
#include "remotebtnitem.h"


RemoteBtnItem::RemoteBtnItem(QWidget *parent) : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    this->lb_img = new QLabel();
    this->lb_text = new QLabel();
    this->lb_img->setObjectName("lb_img");
    this->lb_text->setObjectName("lb_text");
    this->lb_text->setStyleSheet("#lb_text { color:white; font-size:16px; margin-top:5px; }");
    this->lb_img->setAlignment(Qt::AlignCenter);
    this->lb_text->setAlignment(Qt::AlignCenter);

    this->vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(this->lb_img, 4, Qt::AlignBottom);
    vl_total->addWidget(this->lb_text, 3, Qt::AlignTop);

    this->widget_total = new QWidget();
    widget_total->setObjectName("btnImgText");
    widget_total->setLayout(vl_total);

    QVBoxLayout *vl_last = new QVBoxLayout();
    vl_last->setContentsMargins(0,0,0,0);
    vl_last->setSpacing(0);
    vl_last->addWidget(widget_total);
    this->setLayout(vl_last);
}

RemoteBtnItem::~RemoteBtnItem(){

    this->deleteLater();
}

/**
 * @brief BtnImgText::setData 데이터 세팅
 * @param p_urlImg
 * @param p_text
 */
void RemoteBtnItem::setData(QString p_urlImg, QString p_text){
    this->pixmap_icon = GSCommon::getUIPixmapImg(p_urlImg);
    this->lb_img->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    this->lb_img->resize(pixmap_icon->width(), pixmap_icon->height());       //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.
    this->lb_text->setText(p_text);
    if(p_text==""){
        this->lb_text->hide();
    }
}

void RemoteBtnItem::setLabel(QString p_text){
    if(p_text != ""){
        this->lb_text->setText(p_text);
    }
}

/**
 * @brief BtnImgText::setCircle 리모콘의 원모양 버튼 세팅
 * @param p_diameter
 */
void RemoteBtnItem::setCircle(int p_diameter){
    this->setStyleSheet(QString("#btnImgText { background-color:#777777;border-radius:%1px; }").arg(p_diameter/2));
    this->widget_total->setFixedSize(p_diameter,p_diameter);

    if(this->lb_text->text()==""){
        this->vl_total->setAlignment(this->lb_img, Qt::AlignCenter);
    }
}
/**
 * @brief BtnImgText::setRectangle 리모콘의 사각형모양 버튼 세팅 (radius=10 고정)
 * @param p_width
 * @param p_height
 */
void RemoteBtnItem::setRectangle(int p_width, int p_height){
    this->setStyleSheet(QString("#btnImgText { background-color:#4D4D4D;border-radius:%1px; }").arg(10));
    this->widget_total->setFixedSize(p_width,p_height);

    if(this->lb_text->text()==""){
        this->vl_total->setAlignment(this->lb_img, Qt::AlignCenter);
    }
}

/**
 * @brief RemoteBtnItem::mousePressEvent 클릭 시그널 발생
 */
void RemoteBtnItem::mousePressEvent(QMouseEvent*){
    emit clicked();
}




