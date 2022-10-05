#include "qobuz/QobuzSettings.h"

#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ProcRosePlay_withQobuz.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "common/global.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "login/dialoglogin_forqobuz.h"

namespace qobuz {

    /**
     * @brief "타이달 > 설정" 생성자
     * @param parent
     */
    QobuzSettings::QobuzSettings(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::NoneScroll, parent){

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
    QobuzSettings::~QobuzSettings(){

        this->deleteLater();
    }



    /**
     * @brief 화면 활성화
     */
    void QobuzSettings::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        // 타이달 로그인 여부에 따라서
        if(this->is_qobuz_logined()){

            // 로그인 되어 있음
            this->stacked_widget->setCurrentWidget(this->widget_logined);
            this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);

            this->label_userInfo->setText( global.user_forQobuz.getUsername() + tr("  (Account ID)") );
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
    void QobuzSettings::setUIControl_logined(){

        this->label_userInfo = new QLabel();
        this->label_userInfo->setContentsMargins(34,25,10,25);
        this->label_userInfo->setText( global.user_forQobuz.getUsername() + tr("  (Account ID)") );
        //this->label_userInfo->setText( global.user_forQobuz.getUsername() + tr(" (계정 아이디)") );
        this->label_userInfo->setStyleSheet("font-size:18px; color:#ffffff;");

        QPushButton *btn_logout = new QPushButton();
        btn_logout->setFixedHeight(34);
        btn_logout->setText(tr("Log-out"));
        //btn_logout->setText(tr("로그아웃"));
        btn_logout->setStyleSheet(".QPushButton { "
                                        "background-color:transparent; color:#e6e6e6; font-size:15px; "
                                        "border:2px solid #666666; border-radius:17px; padding-left:23px; padding-right:23px; "
                                    "}" );
        btn_logout->setCursor(Qt::PointingHandCursor);
        connect(btn_logout, &QPushButton::clicked, this, &QobuzSettings::slot_clickBtnLogout);

        QHBoxLayout *hBox = new QHBoxLayout();
        hBox->setContentsMargins(0,0,30,0);
        hBox->setSpacing(0);
        hBox->setAlignment(Qt::AlignTop);
        hBox->addWidget(label_userInfo);
        hBox->addWidget(btn_logout, 1, Qt::AlignRight);

        // 아래 라인 1px 넣기
        QWidget *widget_line = new QWidget();
        widget_line->setStyleSheet("background-color:#4d4d4d;");
        widget_line->setFixedHeight(1);

        //------------------------------------------------------------------------------------

        QLabel *label_Streaming = new QLabel();
        label_Streaming->setContentsMargins(34,25,10,25);
        label_Streaming->setText("Streaming");
        label_Streaming->setStyleSheet("font-size:18px; color:#ffffff;");

        this->label_StreamingInfo = new QLabel();
        this->label_StreamingInfo->setContentsMargins(34,25,10,25);

        if(global.user_forQobuz.getHiRes() && global.user_forQobuz.getLossless()){
            this->label_StreamingInfo->setText("24-Bit Hi-Res / up to 192 kHz");
            this->label_StreamingInfo->setStyleSheet("font-size:18px; color:#ffffff;");
        }
        else{
            this->label_StreamingInfo->setText("Pass information section");
            this->label_StreamingInfo->setStyleSheet("font-size:18px; color:#b18658;");
        }

        QHBoxLayout *hBox_streaming = new QHBoxLayout();
        hBox_streaming->setContentsMargins(0,0,30,0);
        hBox_streaming->setSpacing(0);
        hBox_streaming->setAlignment(Qt::AlignTop);
        hBox_streaming->addWidget(label_Streaming);
        hBox_streaming->addWidget(label_StreamingInfo, 1, Qt::AlignRight);

        // 아래 라인 1px 넣기
        QWidget *widget_line2 = new QWidget();
        widget_line2->setStyleSheet("background-color:#4d4d4d;");
        widget_line2->setFixedHeight(1);

        //------------------------------------------------------------------------------------


        QWidget *widget_remain = new QWidget();

        QVBoxLayout *vBox = new QVBoxLayout();
        vBox->setSpacing(0);
        vBox->setContentsMargins(0,0,0,0);
        vBox->addLayout(hBox);
        vBox->addWidget(widget_line, 0, Qt::AlignTop);
        vBox->addLayout(hBox_streaming);
        vBox->addWidget(widget_line2, 0, Qt::AlignTop);
        vBox->addWidget(widget_remain, 1);

        //------------------------------------------------------------------------------------


        // 전체 Widget에 넣기
        this->widget_logined = new QWidget(this);
        this->widget_logined->setLayout(vBox);
        this->widget_logined->setContentsMargins(0,0,0,0);
        this->widget_logined->setMaximumWidth(1200);


        this->stacked_widget->addWidget(this->widget_logined);

    }


    /**
     * @brief 사용자가 타이달 로그인하지 않았을 경우의 화면을 위한 Widget
     */
    void QobuzSettings::setUIControl_notLogin(){
        // 로그인 하지 않았을 때의 화면
        this->widget_notLogin = this->get_createUIControl_notLoginView();
        this->stacked_widget->addWidget(this->widget_notLogin);
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Slot 함수
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 타이달 다이얼로그에서 로그인 성공 이벤트 발생에 대한 처리
     * @details 토스트 메시지를 띄우고, 로그인한 화면으로 전환
     */
    void QobuzSettings::slot_acceptedDialogLogin(){

        // 부모 함수 호출
        AbstractQobuzSubWidget::slot_acceptedDialogLogin();

        if(global.user_forQobuz.flag_rosehome == true){

            emit this->signal_logState_change();
        }
        else{

            // 로그인한 UI로 변경함
            this->setActivePage();

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_HOME);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }
    }


    /**
     * @brief "로그아웃" 버튼 클릭에 대한 처리
     */
    void QobuzSettings::slot_clickBtnLogout(){

        // 로그아웃 처리 요청
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::successLogout, this, &QobuzSettings::slot_successProcLogout);
        proc->request_qobuzLogout();

        // Rose 기기에 Tidal 로그아웃 처리
        ProcRosePlay_withQobuz *procRose = new ProcRosePlay_withQobuz(this);
        qobuz::RoseSessionInfo_forQobuz sessionInfo_init;
        procRose->request_set_session_info(sessionInfo_init);

        // 로그인 상태를 반영하여 페이지 변경함
        global.user_forQobuz.setLogout();
    }


    /**
     * @brief 로그아웃 처리 성공 - UI 변경한다.
     */
    void QobuzSettings::slot_successProcLogout(){

        AbstractQobuzSubWidget::slot_acceptedDialogLogout();

        // 로그인 하지않은 상태의 UI로 변경함
        this->setActivePage();
    }


    void QobuzSettings::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_QOBUZ)){

            global.user.setDeviceChange(false);

            // 로그인한 UI로 변경함
            this->setActivePage();

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_Q_HOME);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);

            if(global.user_forQobuz.isLogined() == true){
                AbstractQobuzSubWidget::slot_acceptedDialogLogin();
            }
            else if(global.user_forQobuz.isLogined() == false){
                AbstractQobuzSubWidget::slot_acceptedDialogLogout();
            }
        }
    }

}
