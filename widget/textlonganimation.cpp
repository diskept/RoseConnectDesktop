#include "textlonganimation.h"

#include <QFontMetrics>
#include <QPainter>
#include <QDebug>
#include <qevent.h>
#include <QTimer>
#include <QTimerEvent>


/**
 * @brief TextLongAnimation::TextLongAnimation 생성자
 * @param p_textAreaWidthMax 텍스트가 보여질 영역의 Max Width
 * @param p_textAreaWidthMin 텍스트가 보여질 영역의 Min Width
 * @param p_textAreaHeight 텍스트가 보여질 영역의 Fixed Height
 * @param parent
 */
TextLongAnimation::TextLongAnimation(int p_textAreaWidthMax, int p_textAreaWidthMin, int p_textAreaHeight, QWidget *parent) : QWidget(parent)
{
    // 기본 UI 레이아웃
    lb_text = new QLabel();
    lb_text->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(lb_text, 0, Qt::AlignHCenter);

    this->setLayout(vl_total);
    this->setFixedHeight(p_textAreaHeight);
    this->setMaximumWidth(p_textAreaWidthMax);
    this->setMinimumWidth(p_textAreaWidthMin);
}

/**
 * @brief TextLongAnimation::setText 멤버변수에 Text를 세팅한다.
 * @param newText
 */
void TextLongAnimation::setText(const QString &newText) {
    this->text = newText;
    this->lb_text->setText(text);
    this->startPosX = 30;    //this->textAreaWidth;   // 시작위치 초기화
}
/**
 * @brief TextLongAnimation::paintEvent [override] 변경된 Position으로 다시 텍스트를 그린다.
 */
void TextLongAnimation::paintEvent(QPaintEvent * /*event*/ ) {

    this->textAreaWidth = this->size().width() + 10;

    if(this->textAreaWidth < this->minimumWidth()){
        this->textAreaWidth = this->minimumWidth();
    }
    else{
    }

    if(this->startPosX == INIT_POINT_X){
        this->startPosX = this->textAreaWidth;
    }

    QPainter painter(this);
    lb_text->setStyleSheet("color:transparent;");

    if(this->textAreaWidth <= this->vl_total->sizeHint().width()){

        if(startPosX < (-this->vl_total->sizeHint().width())){
            this->startPosX = this->textAreaWidth;
        }

        painter.drawText(this->startPosX, this->height()-10, this->text);
        startPosX = startPosX - 1;

    }else{
        lb_text->setStyleSheet("color:white;");
    }
}


/**
 * @brief TextLongAnimation::timerEvent [override] 타이머 돌면서 Position 위치 변경한다.
 * @param event
 */
void TextLongAnimation::timerEvent(QTimerEvent * /*event*/){

}


LabelLongAnimation::LabelLongAnimation(int p_textAreaWidthMax, int p_textAreaWidthMin, int p_textAreaHeight, QWidget *parent) : QWidget(parent)
{
    // 기본 UI 레이아웃
    this->widget_total = new QWidget();
    this->widget_total->setFixedSize(p_textAreaWidthMax, p_textAreaHeight);
    this->widget_total->setStyleSheet("background-color:transparent;");

    this->lb_text = new QLabel(this->widget_total);
    //this->lb_text->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->lb_text->setStyleSheet("background-color:transparent;");
    this->lb_text->setGeometry(0, 0, p_textAreaWidthMin, p_textAreaHeight);
    this->lb_text->setAlignment(Qt::AlignCenter);
    this->lb_text->setText("");

    this->vBox_total = new QVBoxLayout();
    this->vBox_total->setContentsMargins(0,0,0,0);
    this->vBox_total->setSpacing(0);
    this->vBox_total->addWidget(this->widget_total, 0, Qt::AlignTop);

    this->setLayout(this->vBox_total);

    this->setFixedHeight(p_textAreaHeight);
    this->setMaximumWidth(p_textAreaWidthMax);
    this->setMinimumWidth(p_textAreaWidthMin);

    this->timer = new QTimer(this);
    this->timer->setInterval(15);    

    this->timer->stop();
}


LabelLongAnimation::~LabelLongAnimation(){

    if(this->timer->isActive() == true){
        disconnect(this->timer, SIGNAL(timeout()), this, SLOT(tmr_move_text()));
        this->timer->stop();
    }

    this->deleteLater();
}


void LabelLongAnimation::setText(const QString &newText){

    this->text = newText;
    this->lb_text->setText(this->text);
    this->startPosX = 0;
    this->text_length = 0;

    if(this->text.isEmpty() == false){
        this->text_length = this->lb_text->sizeHint().width();
        this->lb_text->setFixedSize(this->text_length, this->height());

        this->textAreaWidth = this->vBox_total->sizeHint().width();

        if(this->text_length > this->textAreaWidth){
            this->falg_connect = true;
            this->lb_text->setGeometry(this->startPosX, -15, 0, 0);

            connect(this->timer, SIGNAL(timeout()), this, SLOT(tmr_move_text()));
            timer->start();
        }
        else{
            this->widget_total->setFixedWidth(this->text_length);

            this->lb_text->setGeometry(this->startPosX, -15, 0, 0);

            disconnect(this->timer, SIGNAL(timeout()), this, SLOT(tmr_move_text()));
        }
    }
}


void LabelLongAnimation::setStyleSheet(const QString &style){

    QString tmpStyle = style;

    this->lb_text->setStyleSheet(tmpStyle);
}


void LabelLongAnimation::tmr_move_text(){

    if(this->textAreaWidth <= this->text_length){

        if(startPosX < (-this->text_length)){
            this->startPosX = this->textAreaWidth - 1;
        }

        this->lb_text->setGeometry(this->startPosX, 0, 0, 0);
        startPosX--;

    }
}
