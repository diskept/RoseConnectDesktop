#include "tidalmain_test.h"
#include "common/global.h"
#include <common/gscommon.h>

TidalMain_Test::TidalMain_Test(QWidget *parent) : AbstractMainContent(parent)
{
    this->setUIControl();

}

void TidalMain_Test::setUIControl(){

    //this->setUIControl_testStretch();

    /*
    // default : 첫번째 서브메뉴
    QJsonObject tmp_data;
    //tmp_data[KEY_PAGE_CODE] = PAGECODE_M_HOME;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_M_CATEGORY;
    this->goToMoveNewOrderPage(tmp_data);
    */
}

void TidalMain_Test::setUIControl_testStretch(){

    QWidget *widget_test = new QWidget();
    widget_test->setMinimumWidth(1000);
    this->stackedWidget_content->addWidget(widget_test);
}

void TidalMain_Test::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

    Q_UNUSED(p_pageCode);
    Q_UNUSED(p_data);
}
