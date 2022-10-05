#include "rosefm_main.h"
#include "common/rosesettings.h"
#include <common/gscommon.h>
#include "widget/toastmsg.h"
#include <QDebug>

RoseFmMain::RoseFmMain(QWidget *parent) : AbstractMainContent(parent)
{
#if defined(Q_OS_WINDOWS)
    //0717 this->mutex.lock();
#endif
    this->setInit();
    this->setUIControl();
#if defined(Q_OS_WINDOWS)
    //0717 this->mutex.unlock();
#endif
}

RoseFmMain::~RoseFmMain(){

}

/**
 * @brief RoseFmMain::setInit : 초기 세팅
 */
void RoseFmMain::setInit(){
    currMainMenuCode = GSCommon::MainMenuCode::RoseFM;

    connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
}

/**
 * @brief RoseFmMain::setUIControl : UI 세팅
 */
void RoseFmMain::setUIControl(){

    // 메뉴 페이지
    this->roseFmHome = new RoseFmHome(this->topMenuBar);

    // 스택에 추가
    this->stackedWidget_content->addWidget(this->roseFmHome);

    // default : 첫번째 서브메뉴
    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_RF_HOME;
    this->goToMoveNewOrderPage(tmp_data);
}

/**
 * @brief VideoMain::slot_search : [슬롯] 검색 요청
 * @param p_text
 */
void RoseFmMain::slot_search(const QString &p_text){

    Q_UNUSED(p_text);
    ToastMsg::show(this, "", tr("Search function is not supported in the RoseFM."));
    //ToastMsg::show(this, "", tr("RoseFM에서는 검색 기능이 지원되지 않습니다."));
}

/**
 * @brief RoseFmMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
 * @param p_pageCode 서브메뉴 코드
 */
void RoseFmMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

    QJsonObject p_jsonObject_titleMain;
    QJsonArray *p_jsonArray_titlSub = new QJsonArray();

    p_jsonObject_titleMain["name"] = tr("RoseFM");
    p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::RoseFM;
    //QJsonObject sub1 { {"name", tr("Home")}, {"code", PAGECODE_RF_HOME} };

    //p_jsonArray_titlSub->push_back(sub1);

    // 부모 virtual 함수 호출
    AbstractMainContent::movePageOnly(p_pageCode, p_data);

    // 자식 override 함수 서술
    this->topMenuBar->setSelectedSubMenu(p_pageCode);
    this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
    this->topMenuBar->setVisible(true);


    if(p_pageCode==PAGECODE_V_HOME){

        if(this->roseFmHome == nullptr){
            this->roseFmHome = new RoseFmHome();
            this->stackedWidget_content->addWidget(roseFmHome);
        }
        this->roseFmHome->get_rose_data();

        stackedWidget_content->setCurrentWidget(roseFmHome);
    }
}
