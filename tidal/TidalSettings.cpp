#include "tidal/TidalSettings.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "common/global.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "login/dialoglogin_fortidal.h"

namespace tidal {

    /**
     * @brief "타이달 > 설정" 생성자
     * @param parent
     */
    TidalSettings::TidalSettings(QWidget *parent) : AbstractTidalSubWidget(MainUIType::NoneScroll, parent){

        this->stacked_widget = new QStackedWidget();
        this->box_contents->addWidget(this->stacked_widget);

        this->setUIControl_notLogin();
        this->setUIControl_logined();

        timer = new QTimer(this);
        timer->setInterval(1000); // 1초
        connect(timer, SIGNAL(timeout()), SLOT(slot_change_logout()));
        this->timer->start();
    }


    /**
     * @brief 소멸자.
     */
    TidalSettings::~TidalSettings(){

        this->deleteLater();
    }


    /**
     * @brief 화면 활성화
     */
    void TidalSettings::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractTidalSubWidget::setActivePage();

        // 타이달 로그인 여부에 따라서
        if(this->is_tidal_logined()){

            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::signal_completeReq_getSubscription, this, &TidalSettings::slot_applyResult_getSubscription);
            proc->request_tidal_getSubscription(global.user_forTidal.getUserId());

            // 로그인 되어 있음
            this->stacked_widget->setCurrentWidget(this->widget_logined);
            this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);

            this->label_userInfo->setText( global.user_forTidal.getUsername() + tr("  (Account ID)") );
            //this->label_userInfo->setText( global.user_forTidal.getUsername() + tr(" (계정 아이디)") );
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
    void TidalSettings::setUIControl_logined(){

        this->label_userInfo = new QLabel();
        this->label_userInfo->setContentsMargins(34,25,10,25);
        this->label_userInfo->setText( global.user_forTidal.getUsername() + tr("  (Account ID)") );
        //this->label_userInfo->setText( global.user_forTidal.getUsername() + tr(" (계정 아이디)") );
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
        connect(btn_logout, &QPushButton::clicked, this, &TidalSettings::slot_clickBtnLogout);

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
    void TidalSettings::setUIControl_notLogin(){
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
    void TidalSettings::slot_acceptedDialogLogin(){
        // 부모 함수 호출
        AbstractTidalSubWidget::slot_acceptedDialogLogin();

        // 로그인한 UI로 변경함
        this->setActivePage();
    }


    /**
     * @brief "로그아웃" 버튼 클릭에 대한 처리
     */
    void TidalSettings::slot_clickBtnLogout(){
        // 로그아웃 처리 요청
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::successLogout, this, &TidalSettings::slot_successProcLogout);
        proc->request_tidalLogout();

        // Rose 기기에 Tidal 로그아웃 처리
        ProcRosePlay_withTidal *procRose = new ProcRosePlay_withTidal(this);
        tidal::RoseSessionInfo_forTidal sessionInfo_init;
        procRose->request_set_session_info(sessionInfo_init);

        // 로그인 상태를 반영하여 페이지 변경함
        global.user_forTidal.set_logoutState();

    }


    /**
     * @brief 로그아웃 처리 성공 - UI 변경한다.
     */
    void TidalSettings::slot_successProcLogout(){

        // 로그인 하지않은 상태의 UI로 변경함
        this->setActivePage();
    }

    void TidalSettings::slot_change_logout(){

        if(global.user_forTidal.isMainOK() == true && global.user_forTidal.isLogined() == true){
            this->login_state = true;
        }

        if(this->login_state == true && global.user_forTidal.isLogined() == false){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_SETTINGS);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }
        else if(this->login_state == false && global.user_forTidal.isLogined() == true){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_HOME);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }

        this->login_state = global.user_forTidal.isLogined();
    }


    void TidalSettings::slot_applyResult_getSubscription(const QString errorMsg, const QJsonObject& jsonObj_session){

        Q_UNUSED(errorMsg);
        if(jsonObj_session.contains("flagOk") && ProcJsonEasy::get_flagOk(jsonObj_session)){

            QString validUntil = ProcJsonEasy::getString(jsonObj_session, "validUntil");

            QJsonObject subscription = ProcJsonEasy::getJsonObject(jsonObj_session, "subscription");
            QString type = ProcJsonEasy::getString(subscription, "type");

            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = validUntil.replace("T", " ");
            splitToken = tmp_split.split(".");

            QString streaming_info = type + " (~ " + splitToken.at(0) + ")";

            this->label_StreamingInfo->setText(streaming_info);
            this->label_StreamingInfo->setStyleSheet("font-size:18px; color:#ffffff;");
        }
        else{
            this->label_StreamingInfo->setText("Pass information section");
            this->label_StreamingInfo->setStyleSheet("font-size:18px; color:#b18658;");
        }
    }

}
