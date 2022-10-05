#include "dialoglogin_fortidal.h"
#include <QPropertyAnimation>
#include <QCryptographicHash>

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "widget/toastmsg.h"
#include "common/ProcJsonEasy.h"
#include "common/sqlitehelper.h"
#include "common/my_lang.h"
#include "tidal/ProcCommon.h"


#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QUrlQuery>
#include <QVBoxLayout>
#include <QWebEngineHistory>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QFocusEvent>
#include "common/ProcJsonEasy.h"


namespace tidal {
    TidalWebPage::TidalWebPage(QWidget *parent) : QWebEnginePage(parent){
        this->parent = parent;
        this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, true);
        this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptCanOpenWindows, true);
        this->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

        connect(this, &TidalWebPage::windowCloseRequested, this, &TidalWebPage::slot_windowCloseRequested);
    }

    void TidalWebPage::javaScriptAlert(const QUrl &securityOrigin, const QString &msg){
        Q_UNUSED(securityOrigin);
        ToastMsg::show(this->parent, "", msg);
    }

    QWebEnginePage* TidalWebPage::createWindow(QWebEnginePage::WebWindowType type){
        Q_UNUSED(type);
        TidalWebPage *tidalWebPage = new TidalWebPage(this->parent);
        tidalWebPage->prev_webPage = this;

        TidalWebView *tmp_view = qobject_cast<TidalWebView*>(this->view());
        tmp_view->setPage(tidalWebPage);

        return tidalWebPage;
    }

    void TidalWebPage::slot_windowCloseRequested(){
        if(this->prev_webPage != nullptr){
            TidalWebView *tmp_view = qobject_cast<TidalWebView*>(this->view());
            tmp_view->setPage(this->prev_webPage);
        }
    }

}


namespace tidal {
    TidalWebView::TidalWebView(QWidget *parent) : QWebEngineView(parent){
        this->parent = parent;
        this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, true);
        this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptCanOpenWindows, true);        
    }

    void TidalWebView::setPage(QWebEnginePage *p_page){
        this->window()->hide();

        QWebEngineView::setPage(p_page);
        this->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
        this->setFocus(Qt::FocusReason::PopupFocusReason);
        this->setFocus(Qt::FocusReason::OtherFocusReason);
        this->show();
        this->window()->show();
    }

}






namespace tidal {

    /**
     * @brief 타이달 서비스 로그인 다이얼로그
     * @param parent
     */
    DialogLogin_forTidal::DialogLogin_forTidal(QWidget *parent) : QDialog(parent) {
        setInit();
        setUIControl();
        setUIControl_topTitle();
        setUIControl_body();
    }


    /**
     * @brief 다이얼로그 관련 초기설정
     */
    void DialogLogin_forTidal::setInit(){

        this->flag_loadUrl = false;
        this->setModal(true);
        this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
        this->setStyleSheet("background-color:transparent;");

        linker = Linker::getInstance();
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief UI 생성 - 화면 구성을 위한 기본 UI 설정
     */
    void DialogLogin_forTidal::setUIControl(){

        this->vl_total = new QVBoxLayout();
        vl_total->setMargin(0);
        vl_total->setContentsMargins(1,1,1,1);
        //vl_total->setAlignment(Qt::AlignTop);
        vl_total->setAlignment(Qt::AlignCenter);


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
    /*void DialogLogin_forTidal::setUIControl_topTitle(){

        // 타이틀
        QLabel *label_title = new QLabel;
        // label_title->setText(tr("TIDAL 로그인"));
        label_title->setText(tr("TIDAL login"));
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

    }*/
    void DialogLogin_forTidal::setUIControl_topTitle(){

        this->btn_back = GSCommon::getUIBtnImg("btn_back", ":/images/icon_back_g.png");
        this->btn_back->setFixedSize(80,70);
        this->btn_back->setCursor(Qt::PointingHandCursor);
        connect(this->btn_back, &QPushButton::clicked, this, &DialogLogin_forTidal::slot_goBack_webView);

        // 타이틀
        QLabel *label_title = new QLabel;
        label_title->setText(tr("TIDAL Log in  "));//label_title->setText(tr("TIDAL 로그인"));
        label_title->setAlignment(Qt::AlignCenter);
        label_title->setStyleSheet("color:#FFFFFF; font-size:22px; background-color:transparent;");

        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":/images/icon_close_gray.png");
        btn_close->setFixedSize(QSize(70,70));
        btn_close->setCursor(Qt::PointingHandCursor);
        connect(btn_close, &QPushButton::clicked, this, &DialogLogin_forTidal::reject);

        QHBoxLayout *hBox = new QHBoxLayout;
        hBox->setContentsMargins(0,0,0,0);
        hBox->setSpacing(0);
        hBox->addWidget(this->btn_back);
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
    /*void DialogLogin_forTidal::setUIControl_body(){

        // 아이디
        lineEdit_email = new QLineEdit;
        lineEdit_email->setMaxLength(30);
        // lineEdit_email->setPlaceholderText(tr("이메일"));
         lineEdit_email->setPlaceholderText(tr("e-mail"));
        lineEdit_email->setFixedHeight(50);
        lineEdit_email->setStyleSheet("font-size:20px; color:#777777; background-color:#212121; border-radius:2px; border:1px solid #4d4d4d; padding-left:23px;");

        // 패스워드
        lineEdit_password = new QLineEdit;
        lineEdit_password->setMaxLength(30);
        // lineEdit_password->setPlaceholderText(tr("비밀번호"));
         lineEdit_password->setPlaceholderText(tr("password"));
        lineEdit_password->setFixedHeight(50);
        lineEdit_password->setEchoMode(QLineEdit::Password);
        lineEdit_password->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;padding-left:23px;");


        //------------------------------------------------------------------

        // 로그인 상태 유지 On/Off 관련
        QLabel *lb_autoLogin = new QLabel;
        //lb_autoLogin->setText(tr("로그인 상태유지"));
         lb_autoLogin->setText(tr("Stay signed in"));
        lb_autoLogin->setStyleSheet("color:#CCCCCC;font-size:18px;");

        // 자동 로그인
        onOff = new OnOffWidget;
        onOff->setValue(true);
        onOff->setCursor(Qt::PointingHandCursor);

        QHBoxLayout *box_autoLogin = new QHBoxLayout;
        box_autoLogin->setMargin(0);
        box_autoLogin->setSpacing(20);
        box_autoLogin->addStretch(1);
        box_autoLogin->addWidget(lb_autoLogin,0, Qt::AlignVCenter);
        box_autoLogin->addWidget(onOff,0, Qt::AlignVCenter);

        //------------------------------------------------------------------

        // 로그인 버튼
        QPushButton *btn_login = new QPushButton;
        // btn_login->setText(tr("로그인"));
         btn_login->setText(tr("Log-in"));
        btn_login->setFixedHeight(55);
        btn_login->setStyleSheet("font-size:20px;color:#FFFFFF;background-color:#B18658;border-radius:27px;margin-left:25px;margin-right:25px;");
        btn_login->setCursor(Qt::PointingHandCursor);
        connect(btn_login, SIGNAL(clicked()), this, SLOT(slot_clickedLogin()));

        //------------------------------------------------------------------

        // 위에서 생성한 위젯 및 서브 레이아웃들을 화면에 추가
        QVBoxLayout *vl_login = new QVBoxLayout;
        vl_login->setSpacing(0);
        vl_login->setContentsMargins(25,45,25,0);
        vl_login->setAlignment(Qt::AlignTop);

        vl_login->addWidget(lineEdit_email);
        vl_login->addSpacing(30);
        vl_login->addWidget(lineEdit_password);
        vl_login->addSpacing(17);
        vl_login->addLayout(box_autoLogin);
        vl_login->addSpacing(60);
        vl_login->addWidget(btn_login);



        // 메인 layout에 추가
        this->vl_total->addLayout(vl_login);


        // 자동 포커스
        lineEdit_email->setFocus(Qt::FocusReason::ActiveWindowFocusReason);

    }*/
    void DialogLogin_forTidal::setUIControl_body(){

        // create webview
        this->tidalWebView = new TidalWebView(this);
        //connect(this->webEngineView, &QWebEngineView::loadFinished, this, &DialogLogin_forBugs::slot_finishLoadingUrl);
        this->tidalWebView->setContentsMargins(0 ,0, 0, 0);
        this->tidalWebView->settings();
        this->vl_total->addWidget(this->tidalWebView, 1);


        TidalWebPage *tidalWebPage = new TidalWebPage(this);
        connect(tidalWebPage, &TidalWebPage::loadFinished, this, &DialogLogin_forTidal::slot_finishLoadingUrl);

        this->tidalWebView->setPage(tidalWebPage);        

        // Request - Tidal API Get
        //   - Authorization Code 발급

        //authorize?
        //client_id=CLIENT_ID
        //restrictSignup%20=%20true
        //lang=en
        //response_type=code
        //geo=KR
        //campaignId=default
        //restrictSignup=true
        //code_challenge=YY1mOvDx7IiloZ3vplovgNBlgqgyUQsS9HXYCHC9s6s
        //code_challenge_method=S256
        //redirect_uri=https://beta.roseaudio.kr/service/application/login?app=bugs/callback;

        //QString str_sha256 = "rose";
        //QByteArray e = QCryptographicHash::hash(str_sha256.toLocal8Bit(),
        //QCryptographicHash::Sha256).toBase64();

        QUrlQuery urlQuery;
        urlQuery.addQueryItem("client_id", global.tidal_client_id);
        urlQuery.addQueryItem("restrictSignup", "true");
        urlQuery.addQueryItem("lang", "kr");
        urlQuery.addQueryItem("response_type", "code");
        urlQuery.addQueryItem("geo", "KR");
        urlQuery.addQueryItem("campaignId", "default");
        urlQuery.addQueryItem("restrictSignup", "true");
        urlQuery.addQueryItem("code_challenge", "YY1mOvDx7IiloZ3vplovgNBlgqgyUQsS9HXYCHC9s6s");
        urlQuery.addQueryItem("code_challenge_method", "S256");
        urlQuery.addQueryItem("redirect_uri", global.tidal_auth_res_redirectUrl);


        QUrl url_login(QString("%1/authorize").arg(global.tidal_auth_url_base));
        url_login.setQuery(urlQuery.query(QUrl::ComponentFormattingOption::FullyEncoded));


        //tidalWebView->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
        tidalWebView->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
        //tidalWebView->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
        tidalWebView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

        tidalWebPage->load(url_login);
    }


    void DialogLogin_forTidal::domainShow(const QVariant &value)
    {
        qDebug()<<value.toString(); //네이버 도메인 확인
    }


    /**
     * @brief DialogLogin_forBugs::slot_finishLoadingUrl
     * @param flagSuccess
     */
    void DialogLogin_forTidal::slot_finishLoadingUrl(bool flagSuccess){
        Q_UNUSED(flagSuccess);

        //qDebug() << this->tidalWebView->page()->url().host() << "\n";
        //qDebug() << this->tidalWebView->page()->url().path() << "\n";

        // 응답 확인
        if(this->tidalWebView->page()->url().host() == "beta.roseaudio.kr"){
            //if(this->tidalWebView->page()->url().path() == "/service/application/login"){
            if(this->tidalWebView->page()->url().path() == "/service/application/callback"){                
                this->tidalWebView->page()->runJavaScript("document.body.textContent", [this](const QVariant &result){
                    QJsonDocument doc = QJsonDocument::fromJson(result.toString().toUtf8());
                    QJsonObject jsonObj_result = doc.object();

                    this->checkResponse_login(jsonObj_result);
                });
            }
        }
    }

    /**
     * @brief Webview의 Back 처리
     */
    void DialogLogin_forTidal::slot_goBack_webView(){
        if(this->tidalWebView->history()->canGoBack()){
            this->tidalWebView->back();
        }
    }


    /**
     * @brief DialogLogin_forBugs::checkResponce_login
     * @param jsonObj_result
     */
    void DialogLogin_forTidal::checkResponse_login(const QJsonObject& jsonObj_result){

        QString code = ProcJsonEasy::getString(jsonObj_result, "code");
        QString error = ProcJsonEasy::getString(jsonObj_result, "error");

        if(!error.isEmpty()){
            this->tidalWebView->back();
            ToastMsg::show(this, "", ProcJsonEasy::getString(jsonObj_result, "error_description"));
        }
        else{
            // 로그인 성공
            //global.user_forTidal.set_tidal_login_authCode(code);
            global.tidalAccessToken = code;
            this->close();

            emit this->signal_successLogin();
        }
    }


    /**
     * @brief DialogLogin_forBugs::applyStyle_backBtn
     */
    void DialogLogin_forTidal::applyStyle_backBtn(){
        if(this->tidalWebView->history()->canGoBack()){
            GSCommon::applyBtnImage(this->btn_back, ":/images/icon_back_w.png");
        }
        else{
            GSCommon::applyBtnImage(this->btn_back, ":/images/icon_back_g.png");
        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : private 내부 체크 및 처리 함수
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 사용자 로그인 정보 입력값 검사
     * @todo TIDAL 로그인 정규식 포맷 요청 to CITECH
     * @return
     */
    /*bool DialogLogin_forTidal::checkUserInput(){
        bool flagOk = true;

        if(lineEdit_email->text().trimmed().isEmpty()){
            flagOk = false;
            ToastMsg::show(this, "",QString(tr("Please enter your TIDAL account email.")));
        }
        else if(lineEdit_password->text().trimmed().isEmpty()){
            flagOk = false;
            //ToastMsg::show(this, "",QString(tr("비밀번호를 입력해주세요.")));
            ToastMsg::show(this, "",QString(tr("Please enter a password.")));
        }

        return flagOk;
    }*/





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------



    /**
     * @brief DialogLogin::slot_clickedLogin : [슬롯]-로그인 버튼 클릭 @n
     * HTTP 요청 - 타이달 로그인 API.
     * @see https://documenter.getpostman.com/view/448610/HXVi?version=latest#11d1026f-3789-480c-aa99-57dae2a357b1
     */
    /*void DialogLogin_forTidal::slot_clickedLogin(){

        if(checkUserInput()){

            // 사용자 로그인 정보
            UserLoginInfo userLoginInfo;
            userLoginInfo.username = lineEdit_email->text().trimmed();
            userLoginInfo.password = lineEdit_password->text().trimmed();
            userLoginInfo.flagSavedLoginInfo = onOff->getCurrentValue();

            ProcCommon *proc = new ProcCommon(this);
            connect(proc, SIGNAL(failedLogin(const QString&)), this, SLOT(slot_failedLogin(const QString&)));
            connect(proc, SIGNAL(successLogin()), this, SLOT(slot_successLogin()));
            proc->request_tidalLogin(userLoginInfo);


        }

    }*/

    /**
     * @brief 로그인 실패한 경우임. Toast메시지를 띄운다.
     * @param errorMsg
     */
    /*void DialogLogin_forTidal::slot_failedLogin(const QString& errorMsg){
        ToastMsg::show(this, "", errorMsg);
    }*/

    /**
     * @brief 로그인 성공! 성공 시그널 발생.
     */
    /*void DialogLogin_forTidal::slot_successLogin(){
        emit successLogin();
        this->close();
    }*/


}


