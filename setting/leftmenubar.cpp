#include "leftmenubar.h"
#include "common/gscommon.h"

#include <QJsonObject>
#include <QVBoxLayout>
#include <QVariant>
#include <QDebug>
const int BTN_WIDTH = 260;      //240;
const int BTN_HEIGHT = 60;



LeftMenuBar::LeftMenuBar(QWidget *parent) : QWidget(parent)
{
    this->setFixedWidth(BTN_WIDTH);
    this->setUIControl();
}

void LeftMenuBar::setUIControl(){
    this->list_subMenuItem.clear();

    QPushButton *btn_myPage = this->createMenuBtn(tr("My page"), PAGECODE_S_MYPAGE);
    QPushButton *btn_friend = this->createMenuBtn(tr("Manage your friends"), PAGECODE_S_FRIEND);
    QPushButton *btn_trackOption = this->createMenuBtn(tr("Audio Settings"), PAGECODE_S_TRACK);//c220511
    QPushButton *btn_lib = this->createMenuBtn(tr("Media Library"), PAGECODE_S_MEDIALIB);
    QPushButton *btn_display = this->createMenuBtn(tr("Display"), PAGECODE_S_DISPLAY);    
    QPushButton *btn_country = this->createMenuBtn(tr("Country Channel Option"), PAGECODE_S_COUNTRY);
    //QPushButton *btn_rosetube = this->createMenuBtn(tr("RoseTube Option"), PAGECODE_S_ROSETUBE);
    QPushButton *btn_video = this->createMenuBtn(tr("Video Option"), PAGECODE_S_VIDEO);
    QPushButton *btn_vu = this->createMenuBtn(tr("VU Option"), PAGECODE_S_VU);
    QPushButton *btn_hdmi = this->createMenuBtn(tr("HDMI Option"), PAGECODE_S_HDMI);//cheon29_src
    QPushButton *btn_timer = this->createMenuBtn(tr("Timer Option"), PAGECODE_S_TIMER);//cheon01_src
    QPushButton *btn_language = this->createMenuBtn(tr("Language Option"), PAGECODE_S_LANGUAGE);//cheon07_language
    QPushButton *btn_inputOutput = this->createMenuBtn(tr("I/O Settings"), PAGECODE_S_INPUTOUTPUT);

    //QPushButton *btn_roseMenu = this->createMenuBtn(tr("Edit device menu"), PAGECODE_S_ROSEMENUEDIT);
    //QPushButton *btn_appMenu = this->createMenuBtn(tr("Edit App Menu"), PAGECODE_S_APPMENUEDIT);
    QPushButton *btn_appInfo = this->createMenuBtn(tr("App Info"), PAGECODE_S_APPINFO);
/*
 *     QPushButton *btn_myPage = this->createMenuBtn(tr("마이페이지"), PAGECODE_S_MYPAGE);
    QPushButton *btn_friend = this->createMenuBtn(tr("친구관리"), PAGECODE_S_FRIEND);
    QPushButton *btn_lib = this->createMenuBtn(tr("미디어 라이브러리"), PAGECODE_S_MEDIALIB);
    QPushButton *btn_display = this->createMenuBtn(tr("디스플레이"), PAGECODE_S_DISPLAY);
    //QPushButton *btn_trackOption = this->createMenuBtn(tr("곡 옵션"), PAGECODE_S_TRACK);
    QPushButton *btn_country = this->createMenuBtn(tr("국가 채널 옵션"), PAGECODE_S_COUNTRY);
    //QPushButton *btn_rosetube = this->createMenuBtn(tr("로즈튜브 옵션"), PAGECODE_S_ROSETUBE);
    QPushButton *btn_video = this->createMenuBtn(tr("비디오 옵션"), PAGECODE_S_VIDEO);
    QPushButton *btn_vu = this->createMenuBtn(tr("VU 옵션"), PAGECODE_S_VU);
    QPushButton *btn_inputOutput = this->createMenuBtn(tr("입출력 설정"), PAGECODE_S_INPUTOUTPUT);
    //QPushButton *btn_roseMenu = this->createMenuBtn(tr("기기 메뉴 편집"), PAGECODE_S_ROSEMENUEDIT);
    //QPushButton *btn_appMenu = this->createMenuBtn(tr("앱 메뉴 편집"), PAGECODE_S_APPMENUEDIT);
    QPushButton *btn_appInfo = this->createMenuBtn(tr("앱 정보"), PAGECODE_S_APPINFO);
*/
    this->list_subMenuItem.append(btn_myPage);
    this->list_subMenuItem.append(btn_friend);
    this->list_subMenuItem.append(btn_trackOption);
    this->list_subMenuItem.append(btn_lib);
    this->list_subMenuItem.append(btn_display);    
    this->list_subMenuItem.append(btn_country);
    //this->list_subMenuItem.append(btn_rosetube);
    this->list_subMenuItem.append(btn_video);
    this->list_subMenuItem.append(btn_hdmi);//cheon29_src
    this->list_subMenuItem.append(btn_timer);//cheon01_src
    this->list_subMenuItem.append(btn_language);//cheon07_language
    this->list_subMenuItem.append(btn_vu);
    this->list_subMenuItem.append(btn_inputOutput);
    //this->list_subMenuItem.append(btn_roseMenu);
    //this->list_subMenuItem.append(btn_appMenu);
    this->list_subMenuItem.append(btn_appInfo);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(btn_myPage);
    vl_total->addWidget(btn_friend);
    vl_total->addWidget(btn_trackOption);
    vl_total->addWidget(btn_lib);
    vl_total->addWidget(btn_display);    
    vl_total->addWidget(btn_country);
    //vl_total->addWidget(btn_rosetube);
    vl_total->addWidget(btn_video);
    vl_total->addWidget(btn_hdmi);  //cheon29_src
    vl_total->addWidget(btn_timer); //cheon01_src
    vl_total->addWidget(btn_language); //cheon07_language
    vl_total->addWidget(btn_vu);
    vl_total->addWidget(btn_inputOutput);
    //vl_total->addWidget(btn_roseMenu);
    //vl_total->addWidget(btn_appMenu);
    vl_total->addWidget(btn_appInfo);
    vl_total->addSpacing(50);

    this->setLayout(vl_total);

    linker = Linker::getInstance();//c221013_1
    // 디폴트 : 첫번째 메뉴
    this->setSelectedSubMenu(PAGECODE_S_MYPAGE);
    connect(linker, SIGNAL(signal_reload_setting_clicked(QString)), this, SLOT(slot_clickedMenu(QString)));//c221013_1
}

QPushButton* LeftMenuBar::createMenuBtn(const QString &text, const QString &code){

    QPushButton *p_btn = new QPushButton();
    p_btn->setProperty(KEY_PAGE_CODE.toStdString().c_str(), code);
    p_btn->setCursor(Qt::PointingHandCursor);
    p_btn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    p_btn->setText(text);
    p_btn->setStyleSheet("QPushButton { color:#919191;background-color:transparent;font-size:18px;font:bold;border:0px;text-align:left;padding-left:34px; } QPushButton:hover { color:#FFFFFF;background-color:#CCCCCC; }");//cheon211114-01

    // 커넥션
    connect(p_btn, SIGNAL(clicked()), this, SLOT(slot_clickedMenu()));

    return p_btn;
}

/**
 * @brief LeftMenuBar::setSelectedSubMenu 해당 메뉴를 선택된 스타일로 세팅하고 나머지는 스타일 해제
 * @param p_subMenuCode
 */
void LeftMenuBar::setSelectedSubMenu(QString p_subMenuCode){

    this->curr_subMenuCode = p_subMenuCode;
    for(int i=0; i<this->list_subMenuItem.count(); i++){
        QString tmp_style = this->list_subMenuItem[i]->styleSheet();
        if(this->list_subMenuItem[i]->property(KEY_PAGE_CODE.toStdString().c_str()).toString()==this->curr_subMenuCode){
            this->list_subMenuItem[i]->setStyleSheet(tmp_style.replace("QPushButton { color:#919191;background-color:transparent;", "QPushButton { color:#FFFFFF;background-color:#B18658;"));
        }else{
            this->list_subMenuItem[i]->setStyleSheet(tmp_style.replace("QPushButton { color:#FFFFFF;background-color:#B18658;", "QPushButton { color:#919191;background-color:transparent;"));
        }
    }
}

/**
 * @brief LeftMenuBar::slot_clickedMenu [SLOT] 메뉴명 버튼 클릭시
 */
void LeftMenuBar::slot_clickedMenu(){

    QString tmp_subMenuCode = sender()->property(KEY_PAGE_CODE.toStdString().c_str()).toString();
    print_debug();
    qDebug() << "tmp_subMenuCode=" << tmp_subMenuCode;
    if(this->curr_subMenuCode != tmp_subMenuCode){

        this->setSelectedSubMenu(tmp_subMenuCode);

        // 현재 선택된 메뉴가 다른 경우에만 메뉴 변경되었다고 시그널 알림
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = this->curr_subMenuCode;
        emit changedSubMenu(tmp_data);
    }
}

void LeftMenuBar::slot_clickedMenu(QString menu){//c221020_1
print_debug();
    //QString tmp_subMenuCode = sender()->property(KEY_PAGE_CODE.toStdString().c_str()).toString();
    //if(this->curr_subMenuCode != menu){

        this->setSelectedSubMenu(menu);

        // 현재 선택된 메뉴가 다른 경우에만 메뉴 변경되었다고 시그널 알림
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = this->curr_subMenuCode;
        emit changedSubMenu(tmp_data);
    //}
}
