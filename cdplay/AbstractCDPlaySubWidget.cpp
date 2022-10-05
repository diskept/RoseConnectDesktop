#include "cdplay/AbstractCDPlaySubWidget.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"

#include <QPixmapCache>
#include <QJsonDocument>
#include <QSettings>


namespace CDplay {

    /**
     * @brief Qobuz 서브페이지 위젯을 위한 부모 클래스
     * @param parent
     */
    AbstractCDPlaySubWidget::AbstractCDPlaySubWidget(MainUIType p_mainUIType, QWidget *parent) : common::AbstractCommonSubWidget(p_mainUIType, parent) {
        // to check memory address number
    }

    /**
     * @brief 소멸자 체크.
     */
    AbstractCDPlaySubWidget::~AbstractCDPlaySubWidget(){

        // to check memory address number
        this->deleteLater();
    }


    void AbstractCDPlaySubWidget::setDataABSPage(QJsonObject p_data){//cheon211018
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


    void AbstractCDPlaySubWidget::slot_changedSubTabUI(const QJsonObject &p_data){//cheon211018
        Q_UNUSED(p_data);
        // Nothing...
    }


    QJsonObject AbstractCDPlaySubWidget::getDataJson(){//cheon211018

        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = this->pageCode;
        tmp_data[KEY_CONTENT_STEP] = this->contentStep;

        return tmp_data;
    }
}
