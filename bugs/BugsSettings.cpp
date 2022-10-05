#include "BugsSettings.h"
#include <QDebug>
#include "common/global.h"
#include "common/gscommon.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"
#include "common/ProcJsonEasy.h"

namespace bugs {


    /**
     * @brief "TIDAL > 홈" 화면을 위한 생성자.
     * @param parent
     */
    BugsSettings::BugsSettings(QWidget *parent) : AbstractBugsSubWidget(VerticalScroll, parent) {

        this->flag_login_check = this->is_bugs_logined();
        this->flag_login_before = this->is_bugs_logined();

        this->timer = new QTimer(this);
        this->timer->setInterval(1000); // 1초
        connect(this->timer, SIGNAL(timeout()), SLOT(slot_change_state()));
        this->timer->start();
    }


    BugsSettings::~BugsSettings(){

        this->deleteLater();
    }


    /**
     * @brief 내가 화면에 보이는 상황에서, 로그인 상태가 변하는 경우에 호출이 되는 함수이다.
     */
    void BugsSettings::applyLoginSuccessState(){

        GSCommon::clearLayout(this->box_contents);
        if(this->is_bugs_logined()){

            // 로그인 되어 있음
            this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            this->setUIControl_logined();


            // 화면에 노출 데이터 노출
            this->label_userInfo->setText(global.user_forBugs.getNickName());
            QString bugs_productName = global.user_forBugs.getProductName();
            if(bugs_productName.isEmpty()){
                this->label_ticketName->setText("사용중인 이용권이 없습니다.");
            }
            else{
                this->label_ticketName->setText(bugs_productName);
            }

        }
        else{
            // 로그인 안되어 있음
            this->setUIControl_notLogin();
        }
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 사용자가 Bugs 로그인했을 경우의 화면을 위한 Widget
     */
    void BugsSettings::setUIControl_logined(){

        this->label_userInfo = new QLabel();
        this->label_userInfo->setContentsMargins(0,25,10,25);
        this->label_userInfo->setText( global.user_forTidal.getUsername() + " (계정 아이디)" );
        this->label_userInfo->setStyleSheet("font-size:18px; color:#ffffff;");

        QPushButton *btn_logout = new QPushButton();
        btn_logout->setContentsMargins(0,0,0,0);
        btn_logout->setFixedHeight(34);
        btn_logout->setText("로그아웃");
        btn_logout->setStyleSheet(".QPushButton { "
                                        "background-color:transparent; color:#e6e6e6; font-size:15px; "
                                        "border:2px solid #666666; border-radius:17px; padding-left:23px; padding-right:23px; "
                                    "}" );
        btn_logout->setCursor(Qt::PointingHandCursor);
        connect(btn_logout, &QPushButton::clicked, this, &BugsSettings::slot_clickBtnLogout);

        QHBoxLayout *hBox = new QHBoxLayout();
        hBox->setContentsMargins(34,0,50,0);
        hBox->setSpacing(0);
        hBox->setAlignment(Qt::AlignTop);
        hBox->addWidget(label_userInfo);
        hBox->addWidget(btn_logout, 1, Qt::AlignRight);

        //------------------------------------------------------------------------------------

        // 아래 라인 1px 넣기
        QWidget *widget_line = new QWidget();
        widget_line->setStyleSheet("background-color:#4d4d4d;");
        widget_line->setFixedHeight(1);

        //------------------------------------------------------------------------------------

        QLabel *tmp_label_ticket = new QLabel();
        tmp_label_ticket->setContentsMargins(0,25,10,25);
        tmp_label_ticket->setText("사용중인 이용권");
        tmp_label_ticket->setStyleSheet("font-size:18px; color:#ffffff;");

        this->label_ticketName = new QLabel();
        this->label_ticketName->setContentsMargins(0,25,0,25);
        this->label_ticketName->setText("이용권 정보 부분");
        this->label_ticketName->setStyleSheet("font-size:18px; color:#b18658;");

        QHBoxLayout *hBox_2 = new QHBoxLayout();
        hBox_2->setContentsMargins(34,0,50,0);
        hBox_2->setSpacing(0);
        hBox_2->setAlignment(Qt::AlignTop);
        hBox_2->addWidget(tmp_label_ticket);
        hBox_2->addWidget(this->label_ticketName, 1, Qt::AlignRight);


        // 아래 라인 1px 넣기
        QWidget *widget_line_2 = new QWidget();
        widget_line_2->setStyleSheet("background-color:#4d4d4d;");
        widget_line_2->setFixedHeight(1);

        //------------------------------------------------------------------------------------

        QWidget *widget_remain = new QWidget();

        QVBoxLayout *vBox = new QVBoxLayout();
        vBox->setSpacing(0);
        vBox->setContentsMargins(0,0,0,0);
        vBox->addLayout(hBox);
        vBox->addWidget(widget_line, 0, Qt::AlignTop);
        vBox->addLayout(hBox_2);
        vBox->addWidget(widget_line_2, 0, Qt::AlignTop);
        vBox->addWidget(widget_remain, 1);

        //------------------------------------------------------------------------------------


        // 전체 Widget에 넣기
        QWidget *widget_logined = new QWidget(this);
        widget_logined->setLayout(vBox);
        widget_logined->setContentsMargins(0,0,0,0);
        widget_logined->setMaximumWidth(1200);

        this->box_contents->addWidget(widget_logined, 0, Qt::AlignTop);
    }


    void BugsSettings::slot_change_state(){

        this->flag_login_before = this->flag_login_check;
        this->flag_login_check = this->is_bugs_logined();

        if(this->flag_login_before == false && this->flag_login_check == true){

            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_BUGS___HOME);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }
    }
}
