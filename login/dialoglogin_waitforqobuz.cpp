#include "dialoglogin_waitforqobuz.h"
#include <QPropertyAnimation>
#include <QCryptographicHash>

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "widget/toastmsg.h"
#include "common/ProcJsonEasy.h"
#include "common/sqlitehelper.h"
#include "common/my_lang.h"
#include "qobuz/ProcCommon_forQobuz.h"



namespace qobuz {



    /**
     * @brief 코부즈 서비스 로그인 다이얼로그
     * @param parent
     */
    DialogLogin_WaitforQobuz::DialogLogin_WaitforQobuz(QWidget *parent) : QDialog(parent) {
        setInit();
        setUIControl();
        setUIControl_topTitle();
        setUIControl_body();
    }


    /**
     * @brief 다이얼로그 관련 초기설정
     */
    void DialogLogin_WaitforQobuz::setInit(){
        this->setModal(true);
        this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
        this->setStyleSheet("background-color:transparent;");

    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief UI 생성 - 화면 구성을 위한 기본 UI 설정
     */
    void DialogLogin_WaitforQobuz::setUIControl(){

        this->vl_total = new QVBoxLayout();
        vl_total->setMargin(0);
        vl_total->setContentsMargins(1,1,1,1);
        vl_total->setAlignment(Qt::AlignTop);


        QWidget *widget_total = new QWidget();
        widget_total->setLayout(vl_total);
        widget_total->setObjectName("widget_total");
        widget_total->setStyleSheet("#widget_total {background-color:#333333;border-radius:4px;border:1px solid #4D4D4D;}");

        QVBoxLayout *vboxlayout = new QVBoxLayout();
        vboxlayout->setMargin(0);
        vboxlayout->setSpacing(0);
        vboxlayout->addWidget(widget_total);

        this->setLayout(vboxlayout);

    }



    /**
     * @brief UI 생성 - 다이얼로그의 Top Title 파트
     */
    void DialogLogin_WaitforQobuz::setUIControl_topTitle(){

        // 타이틀
        QLabel *label_title = new QLabel;
        // label_title->setText(tr("Qobuz 로그인"));
        label_title->setText(tr("Qobuz Login"));
        label_title->setAlignment(Qt::AlignCenter);
        label_title->setStyleSheet("color:#FFFFFF; font-size:22px; background-color:transparent; padding-left:70px;");

        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
        btn_close->setFixedSize(QSize(70,70));
        btn_close->setCursor(Qt::PointingHandCursor);
        connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

        QHBoxLayout *hBox = new QHBoxLayout;
        hBox->setMargin(0);
        hBox->setSpacing(0);
        hBox->addWidget(label_title,1);
        hBox->addWidget(btn_close);

        QWidget *widget_topTitle = new QWidget;
        widget_topTitle->setObjectName("widget_top");
        widget_topTitle->setLayout(hBox);
        widget_topTitle->setStyleSheet("#widget_top { background-color:transparent; border-bottom:1px solid #404040; }");

        // 메인 layout에 추가
        this->vl_total->addWidget(widget_topTitle);

    }


    /**
     * @brief UI 생성 - 다이얼로그의 바디 파트
     */
    void DialogLogin_WaitforQobuz::setUIControl_body(){

        // 로그인 상태 유지 On/Off 관련
        QLabel *lb_autoLogin = new QLabel;
        lb_autoLogin->setText(tr("Preparing QOBUZ service."));
        lb_autoLogin->setStyleSheet("color:#CCCCCC;font-size:22px;");
        lb_autoLogin->setAlignment(Qt::AlignCenter);


        QHBoxLayout *box_autoLogin = new QHBoxLayout;
        box_autoLogin->setMargin(0);
        box_autoLogin->setSpacing(20);
        box_autoLogin->addWidget(lb_autoLogin, 0, Qt::AlignVCenter);
        //------------------------------------------------------------------


        // 로그인 버튼
        QPushButton *btn_login = new QPushButton;
        btn_login->setText(tr("Confirm"));
        btn_login->setFixedHeight(55);
        btn_login->setStyleSheet("font-size:20px;color:#FFFFFF;background-color:#B18658;border-radius:27px;margin-left:25px;margin-right:25px;");
        btn_login->setCursor(Qt::PointingHandCursor);
        connect(btn_login, SIGNAL(clicked()), this, SLOT(slot_successLogin()));

        //------------------------------------------------------------------

        // 위에서 생성한 위젯 및 서브 레이아웃들을 화면에 추가
        QVBoxLayout *vl_login = new QVBoxLayout;
        vl_login->setSpacing(0);
        vl_login->setContentsMargins(25,45,25,0);
        vl_login->setAlignment(Qt::AlignTop);

        vl_login->addLayout(box_autoLogin);
        vl_login->addSpacing(60);
        vl_login->addWidget(btn_login);

        // 메인 layout에 추가
        this->vl_total->addLayout(vl_login);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief Dialog close.
     */
    void DialogLogin_WaitforQobuz::slot_successLogin(){
        this->close();
    }


}


