#include "gscommon.h"
#include "global.h"
#include <QFontMetrics>
#include <QLabel>
#include <QLayoutItem>
#include <QMessageBox>
#include <QPushButton>
#include <QFileInfo>
#include <QTime>
#include <QDebug>

QMutex mtx;

GSCommon::GSCommon(){
}

/**
  * @brief GSCommon::getTextCutFromLabelWidth 텍스트 길이가 잘리는경우 (Label의 width보다 텍스트가 긴 경우) 알아서 생략(...)표시 하여 텍스트를 반환한다.
  * @param p_text 원본 텍스트
  * @param p_width 텍스트를 올릴 QLabel의 width 값
  * @param p_font 텍스트를 올릴 QLabel의 QFont 값
  * @return QLabel사이즈에 맞게 생략(...)표시가 되어 텍스트가 반환된다.
  */
QString GSCommon::getTextCutFromLabelWidth(QString p_text, int p_width, const QFont &p_font){

    int tmp_width = p_width > 5 ? p_width - 5 : p_width;

    QFontMetrics metrics(p_font);
    QString elidedText = metrics.elidedText(QString(p_text), Qt::ElideRight, tmp_width);
    //p_lb->setText(elidedText);
    return elidedText;
}

/**
  * @brief GSCommon::clearLayout 해당 레이아웃안의 모든 item을 제거한다.
  * @param layout
  */
void GSCommon::clearLayout(QLayout *layout)
{

    //mtx.lock();
    QLayoutItem *item;

    if(layout != 0){
        while ((item = layout->takeAt(0))){


            delete item->widget();
            delete item;

        }
    }
    else{
        //qDebug() << "GSCommon::clearLayout--- layout null!!!!";//c220528
    }
    //mtx.unlock();

    /*while ((item = layout->takeAt(0))){
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }*/
}

void GSCommon::clearLayout_album(QLayout *layout, int line_cnt, int page_lmt)//c22040301
{

    //QMutex mtx;
   // mtx.lock();
    QLayoutItem *item;
    qDebug() << "layout->count() = " << layout->count();
    int i = 0;
    int l_cnt = line_cnt*(page_lmt);
    if(layout != 0){
        while ((item = layout->takeAt(0))){
            //qDebug() << "i= " << i;
            if((i+1) >= l_cnt) break;

            i++;
            delete item->widget();
            delete item;

        }
    }
    else{
        qDebug() << "GSCommon::clearLayout--- layout null!!!!";
    }
   // mtx.unlock();

}
void GSCommon::clearLayout_album_onePage(QLayout *layout, int start_index, int end_index)//c220415
{

    //QMutex mtx;
   // mtx.lock();

    QLayoutItem *item;
    //print_debug();
    //qDebug() << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    //qDebug() << "layout->count() = " << layout->count();
    //qDebug() << "start_index = " << start_index;
    //qDebug() << "end_index = " << end_index;

    int l_cnt =(end_index) -  start_index;
    if(layout != 0 && end_index <= layout->count()){
        while (1){
            //qDebug() << "i= " << i;
            if((l_cnt--)==0) break;
            item = layout->takeAt(start_index);
            delete item->widget();
            delete item;

        }
        //qDebug() << "after---layout->count() = " << layout->count();
    }

    else{
        //qDebug() << "GSCommon::clearLayout--- layout null!!!!";
    }
   // mtx.unlock();

}

void GSCommon::clearLayout_new(QLayout *layout){
    if (!layout)
        return;

    while (auto item = layout->takeAt(0)) {
        item->widget()->deleteLater();
        clearLayout_new(item->layout());
    }
}

/**
  * @brief GSCommon::getUIPixmapImg 리소스이미지를 올린 QPixmap을 반환한다.
  * @param p_url 리소스이미지 경로
  * @param parent
  * @return QLabel
  */
QPixmap* GSCommon::getUIPixmapImg(QString p_url){

    //QImage *img = new QImage();
    QImage img;
    QPixmap *pixmap_icon = new QPixmap();
    if(img.load(p_url)){
        *pixmap_icon = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon = pixmap_icon->scaled(img.width(), img.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }else{
        if(!p_url.isEmpty()){//c220804
            QMessageBox::about(0, "Image load Error", p_url+":Image load Error[Common]...Retry.. (p_url :"  + p_url);
        }else{
            qDebug() << "GSCommon::getUIPixmapImg===============================================================================p_url: is null";
        }

    }
    return pixmap_icon;
}

QPixmap* GSCommon::getUIPixmapImg_WH(QString p_url,int w,int h){//c220816

    //QImage *img = new QImage();
    QImage img;
    QPixmap *pixmap_icon = new QPixmap();
    if(img.load(p_url)){
        *pixmap_icon = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon = pixmap_icon->scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }else{
        if(!p_url.isEmpty()){//c220804
            QMessageBox::about(0, "Image load Error", p_url+":Image load Error[Common]...Retry.. (p_url :"  + p_url);
        }else{
            qDebug() << "GSCommon::getUIPixmapImg===============================================================================p_url: is null";
        }

    }
    return pixmap_icon;
}

/**
  * @brief GSCommon::getUILabelImg 리소스이미지를 올린 QLabel을 반환한다.
  * @param p_url 리소스이미지 경로 (ex> :/images/playBar/net_play_ico.png )
  * @param parent
  * @return QLabel
  */
QLabel* GSCommon::getUILabelImg(QString p_url, QWidget *parent){

    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(p_url);
    QLabel *lb_icon = new QLabel(parent);
    lb_icon->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    lb_icon->resize(pixmap_icon->width(), pixmap_icon->height());       //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.
    return lb_icon;
}
QLabel* GSCommon::getUILabelImg_search(QString p_url, QWidget *parent){

    QImage img;
    QPixmap *pixmap_icon = new QPixmap();
    if(img.load(p_url)){
        *pixmap_icon = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_icon = pixmap_icon->scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);                //이미지 사이즈 조절
    }else{
        if(!p_url.isEmpty()){//c220804
            QMessageBox::about(0, "Image load Error", p_url+":Image load Error[Common]...Retry.. (p_url :"  + p_url);
        }else{
            qDebug() << "GSCommon::getUIPixmapImg===============================================================================p_url: is null";
        }
    }

    QLabel *lb_icon = new QLabel(parent);
    lb_icon->setPixmap(*pixmap_icon);                                   //버퍼에 있는 이미지를 QLabel에 출력
    //lb_icon->resize(pixmap_icon->width(), pixmap_icon->height());       //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.
    return lb_icon;
}

/**
  * @brief GSCommon::getUIBtnImg 리소스이미지를 올린 QPushButton을 반환한다.
  * @param p_objName [필수값] stylesheet에 아이디값을 주어 이미지를 올리는 구조로 되어 필수값 요망. 후 size 세팅 필요함
  * @param p_url 리소스이미지 경로
  * @param parent
  * @return QPushButton
  */
QPushButton* GSCommon::getUIBtnImg(QString p_objName, QString p_url, QWidget *parent){
     QPushButton *btn_icon = new QPushButton(parent);
     btn_icon->setObjectName(p_objName);
     btn_icon->setStyleSheet("#" + p_objName + " { background-color:rgba(0,0,0,0); background-image: url('" + p_url + "'); background-repeat:no-repeat; background-position:center center; } ");
     return btn_icon;
}

/**
 * @brief PushButton 에 배경 이미지 설정함
 * @param p_btn
 * @param p_url
 */
void GSCommon::applyBtnImage(QPushButton *p_btn, QString p_url){
     p_btn->setStyleSheet(".QPushButton { background-color:rgba(0,0,0,0);background-image: url('" + p_url + "');background-repeat:no-repeat; background-position:center center; } ");
}



/**
 * @brief GSCommon::checkFileExists : 파일 존재 여부 확인
 * @param p_filePath Qstring filePath
 * @return bool if true, exists
 */
bool GSCommon::checkFileExists(const QString &p_filePath){
    QFileInfo file(p_filePath);
    if(file.exists() && file.isFile()){
        return true;
    }

    return false;
}


/**
 * @brief GSCommon::getUIControlTitleForSetting 설정 메뉴에서의 타이틀 UI 생성
 * @param p_title
 * @return
 */
QWidget* GSCommon::getUIControlTitleForSetting(QString p_title){

    // Text
    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;font:bold;");

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,33,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);

    QWidget *widget_title = new QWidget();
    widget_title->setObjectName("widget_title");
    widget_title->setContentsMargins(0,0,0,0);
    widget_title->setLayout(hl_lb);
    widget_title->setStyleSheet("#widget_title { background-color:#333333; } QLabel { background-color:transparent; }");

    return widget_title;
}

/**
 * @brief getHorizontalBar 가로 Bar UI 생성
 * @param p_color 색상값4자리 또는 6자리 (#포함하고 입력)
 * @param p_height 높이값
 * @return QWidget
 */
QWidget* GSCommon::getHorizontalBar(QString p_color, int p_height){
    QWidget *widget = new QWidget();
    widget->setFixedHeight(p_height);
    widget->setStyleSheet("background-color:" + p_color + ";");
    return widget;
}

/**
 * @brief GSCommon::convertTimeFormat : 시간 포맷 변경
 * @param p_milliseconds int 밀리초
 * @return QString h:mm:ss
 */
QString GSCommon::convertTimeFormat(const int &p_time, bool p_flagIsMSec){

    int tmp_timeMSec = p_flagIsMSec ? p_time : p_time*1000;
    QString tmp_strDuration;

    QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_timeMSec); // QTime("00:03:27.700")
    if(tmp_timeMSec >= (60*60*1000)){
        tmp_strDuration = tmp_time.toString("h:mm:ss");
    }else{
        tmp_strDuration = tmp_time.toString("mm:ss");
    }

    return tmp_strDuration;
}
