#include "videofolder.h"


#include <common/gscommon.h>

VideoFolder::VideoFolder(QWidget *parent) : AbstractPage(parent)
{

}

/**
 * @brief VideoFolder::changedSubTab [SLOT][override]
 * @param p_data
 */
void VideoFolder::slot_changedSubTabUI(const QJsonObject &p_data){
    QString tmp_step = p_data[KEY_PAGE_CODE].toString();

    QJsonObject tmp_data = this->getDataJson();
    tmp_data[KEY_CONTENT_STEP] = tmp_step;

    emit clickedSubTab(tmp_data);
}


/**
 * @brief VideoFolder::setData [override]
 * @param p_data
 */
void VideoFolder::setDataABSPage(QJsonObject p_data){
    // 공통 데이터 세팅
    AbstractPage::setDataABSPage(p_data);
}

/**
 * @brief VideoFolder::getDataJson [override]
 * @return
 */
QJsonObject VideoFolder::getDataJson(){
    // 공통 데이터 반환
    QJsonObject tmp_data = AbstractPage::getDataJson();


    return tmp_data;
}

