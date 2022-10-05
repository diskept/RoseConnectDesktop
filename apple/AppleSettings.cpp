#include "apple/AppleSettings.h"

#include "apple/ProcRosePlay_withApple.h"

#include "common/global.h"
#include "common/gscommon.h"

//#include "login/dialoglogin_forApple.h"

#include "widget/toastmsg.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

namespace apple {

    /**
     * @brief "APPLE > 설정" 생성자
     * @param parent
     */
    AppleSettings::AppleSettings(QWidget *parent) : AbstractAppleSubWidget(MainUIType::NoneScroll, parent)
    {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));

        this->stacked_widget = new QStackedWidget();
        this->box_contents->addWidget(this->stacked_widget);

        this->setUIControl_notLogin();
        this->setUIControl_logined();
    }

    /**
     * @brief 소멸자.
     */
    AppleSettings::~AppleSettings(){

        this->deleteLater();
    }

    /**
     * @brief 화면 활성화
     */
    void AppleSettings::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractAppleSubWidget::setActivePage();

        // 타이달 로그인 여부에 따라서
        if(global.user_forApple.isLogined()){

            // 로그인 되어 있음
            this->stacked_widget->setCurrentWidget(this->widget_logined);
            this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);

            //this->label_userInfo->setText( global.user_forQobuz.getUsername() + tr("  (Account ID)") );
            //this->label_userInfo->setText( global.user_forQobuz.getUsername() + tr(" (계정 아이디)") );
        }
        else{
            // 로그인 안되어 있음
            this->stacked_widget->setCurrentWidget(this->widget_notLogin);
            this->box_contents->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 사용자가 타이달 로그인했을 경우의 화면을 위한 Widget
     */
    void AppleSettings::setUIControl_logined(){

    }


    /**
     * @brief 사용자가 타이달 로그인하지 않았을 경우의 화면을 위한 Widget
     */
    void AppleSettings::setUIControl_notLogin(){
        // 로그인 하지 않았을 때의 화면
        this->widget_notLogin = this->get_createUIControl_notLoginView();
        this->stacked_widget->addWidget(this->widget_notLogin);
    }
}
