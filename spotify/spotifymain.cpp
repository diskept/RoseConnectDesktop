#include "spotifymain.h"

#include <QDebug>

#include <common/gscommon.h>

SpotifyMain::SpotifyMain(QWidget *parent) : AbstractMainContent(parent)
{    
    this->setUIControl();
}


/**
 * @brief SpotifyMain::setUIControl : UI 세팅
 */
void SpotifyMain::setUIControl(){

    // 메뉴 페이지




    // default : 첫번째 서브메뉴
    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_SP_HOME;
    this->goToMoveNewOrderPage(tmp_data);

}


/**
 * @brief SpotifyMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
 * @param p_pageCode
 */
void SpotifyMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

    QJsonObject p_jsonObject_titleMain;
    QJsonArray *p_jsonArray_titlSub = new QJsonArray();

    p_jsonObject_titleMain["name"] = tr("Spotify");
    p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Spotify;
    QJsonObject sub1 { {"name", tr("Home")}, {"code", PAGECODE_SP_HOME} };
    QJsonObject sub2 { {"name", tr("Explore")}, {"code", "search"} };
    QJsonObject sub3 { {"name", tr("Subcribe")}, {"code", "sub"} };
    p_jsonArray_titlSub->push_back(sub1);
    //p_jsonArray_sub->push_back(sub2);
    //p_jsonArray_sub->push_back(sub3);

    this->topMenuBar->setVisible(true);

    // 부모 virtual 함수 호출
    AbstractMainContent::movePageOnly(p_pageCode, p_data);


    // 자식 override 함수 서술
    this->topMenuBar->setSelectedSubMenu(p_pageCode);
    this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
}
