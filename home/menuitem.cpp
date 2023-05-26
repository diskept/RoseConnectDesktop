#include "home/menuitem.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include "widget/toastmsg.h"
#include "common/global.h"

//const int TEXTSIZE_W = 175;
/**
 * @brief MenuItem::MenuItem 생성자
 * @param p_menuName
 * @param p_menuCode
 * @param p_iconPath
 * @param parent
 */
MenuItem::MenuItem(const QString p_menuName, const QString p_menuCode, const QString p_iconPath, QWidget *parent) : QWidget(parent) {

    this->setCursor(Qt::PointingHandCursor);

    // 멤버변수 세팅
    this->menuName = p_menuName;
    this->menuCode = p_menuCode;
    QString iconPath = p_iconPath;
    //QString iconPath_selected = p_iconPath.chopped(4) + "_gold" + p_iconPath.right(4);

    QImage *img = new QImage();
    this->pixmap_buffer = new QPixmap();
    //this->pixmap_buffer_selected = new QPixmap();
    if(img->load(iconPath)){
        *pixmap_buffer = QPixmap::fromImage(*img);                                  //이미지를 버퍼에 옮긴다.
        *pixmap_buffer = pixmap_buffer->scaled(img->width(), img->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);        //이미지 사이즈 조절
    }
    /*if(img->load(iconPath_selected)){
        *pixmap_buffer_selected = QPixmap::fromImage(*img);
        *pixmap_buffer_selected = pixmap_buffer_selected->scaled(img->width(), img->height());
    }*/

    this->lb_icon = new QLabel();
    this->lb_icon->setPixmap(*pixmap_buffer);               //버퍼에 있는 이미지를 QLabel에 출력
    this->lb_icon->resize(pixmap_buffer->width(), pixmap_buffer->height()); //QLabel의 크기를 이미지 사이즈에 맞추어 조절한다.

    this->lb_text = new QLabel();
    this->lb_text->setContentsMargins(10,0,0,0);
    this->lb_text->setObjectName("lb_menuName");
    this->lb_text->setText(p_menuName);
    this->lb_text->setStyleSheet("#lb_menuName { color:white; font-size:16px;font:bold;}"); //c230223_1

    this->hl_menu = new QHBoxLayout;
    this->hl_menu->setContentsMargins(0,0,0,0);
    //this->hl_menu->addSpacing(25);
    this->hl_menu->addWidget(this->lb_icon);
    this->hl_menu->addWidget(this->lb_text,1,Qt::AlignLeft);

    QWidget *widget = new QWidget();
    widget->setObjectName("menuItem");
    widget->setLayout(hl_menu);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(20,0,20,0);
    vl_total->addWidget(widget);

    this->setStyleSheet("#menuItem QLabel { font-size:14px; } #menuItem:enabled { background-color:#000000; border:1px solid #000000; border-radius:3px;} #menuItem:hover:enabled { background-color:#b18658; border:1px solid #b18658; border-radius:3px;} ");
    this->setFixedHeight(40);
    this->setLayout(vl_total);
    this->setProperty("menuCode", this->menuCode);//c230518
}


MenuItem::~MenuItem(){

    this->deleteLater();
}


/**
 * @brief MenuItem::mousePressEvent 클릭 시그널 발생
 */
//void MenuItem::mousePressEvent(QMouseEvent *event){
void MenuItem::mouseReleaseEvent(QMouseEvent *event){

    Q_UNUSED(event);
    if(global.window_activate_flag) {//c221001_1
    }else {
        return;
    }
    //if(global.dialog_delay == nullptr) return;//c230429
    //if(!global.dialog_delay->isHidden()) return;//c230429
    ToastMsg::delay(this,"", tr("delay"), 2000);
    if(global.isDrawingMainContent == true){
        emit clicked(this->menuCode);
    }
}


void MenuItem::enterEvent(QEvent *event)//c230518
{
    Q_UNUSED(event);
    //qDebug() << "Mouse entered the widget";
    emit this->signal_ttt();
    // Perform actions when the mouse enters the widget
}

void MenuItem::leaveEvent(QEvent *event)//c230518
{
    Q_UNUSED(event);
    //qDebug() << "Mouse left the widget";
    // Perform actions when the mouse leaves the widget
}

void MenuItem::setLoginedSelectedMenu(){//c230223_2

    this->flagSelected = false;
    //this->lb_icon->setPixmap(*pixmap_buffer);
    //this->lb_icon->resize(pixmap_buffer->width(), pixmap_buffer->height());
    this->lb_text->setStyleSheet("#lb_menuName { color:#cccccc; font:bold; }");
    this->setStyleSheet("#menuItem QLabel { font-size:16px;  } #menuItem:enabled { background-color:#000000; border:1px solid #000000; border-radius:3px;} #menuItem:hover:enabled { background-color:#999999; border:1px solid #999999; border-radius:3px;} ");//cheon211114-01
}


void MenuItem::setSelectedMenu(){

    this->flagSelected = true;
    //this->lb_icon->setPixmap(*pixmap_buffer_selected);
    //this->lb_icon->resize(pixmap_buffer_selected->width(), pixmap_buffer_selected->height());
    //this->lb_text->setStyleSheet("#lb_menuName { color:#B18658;   }");
    this->setStyleSheet("#menuItem QLabel { font-size:14px; } #menuItem:enabled { background-color:#b18658; border:1px solid #b18658; border-radius:3px;} #menuItem:hover:enabled { background-color:#333333; border:1px solid #333333; border-radius:3px;} ");
}


void MenuItem::setUnSelectedMenu(){

    this->flagSelected = false;
    //this->lb_icon->setPixmap(*pixmap_buffer);
    //this->lb_icon->resize(pixmap_buffer->width(), pixmap_buffer->height());
    //this->lb_text->setStyleSheet("#lb_menuName { color:white;  }");
    this->setStyleSheet("#menuItem QLabel { font-size:14px; } #menuItem:enabled { background-color:#000000; border:1px solid #000000; border-radius:3px;} #menuItem:hover:enabled { background-color:#999999; border:1px solid #999999; border-radius:3px;} ");//cheon211114-01
}


void MenuItem::showMenuName(){

    this->lb_text->setVisible(true);
}


void MenuItem::hideMenuName(){

    this->lb_text->setVisible(false);
}

