#include "abstractpage.h"

#include <QJsonObject>

#include <common/gscommon.h>

AbstractPage::AbstractPage(QWidget *parent) : QWidget(parent)
{

}


void AbstractPage::setDataABSPage(QJsonObject p_data){
    this->pageCode = "";
    this->contentStep = "";

    // 데이터 세팅
    if(p_data.contains(KEY_PAGE_CODE)){
        this->pageCode = p_data[KEY_PAGE_CODE].toString();
    }
    if(p_data.contains(KEY_CONTENT_STEP)){
        this->contentStep = p_data[KEY_CONTENT_STEP].toString();
    }
}


void AbstractPage::slot_changedSubTabUI(const QJsonObject &p_data){
    Q_UNUSED(p_data);
    // Nothing...
}


QJsonObject AbstractPage::getDataJson(){

    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = this->pageCode;
    tmp_data[KEY_CONTENT_STEP] = this->contentStep;

    return tmp_data;
}
