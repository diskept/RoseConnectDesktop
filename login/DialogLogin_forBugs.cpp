#include "DialogLogin_forBugs.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QUrlQuery>
#include <QVBoxLayout>
#include <QWebEngineHistory>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QFocusEvent>
#include "common/global.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "common/ProcJsonEasy.h"
#include "widget/toastmsg.h"


namespace bugs {
    BugsWebPage::BugsWebPage(QWidget *parent) : QWebEnginePage(parent){
        this->parent = parent;
        this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, true);
        this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptCanOpenWindows, true);
        this->profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);

        connect(this, &BugsWebPage::windowCloseRequested, this, &BugsWebPage::slot_windowCloseRequested);
    }

    void BugsWebPage::javaScriptAlert(const QUrl &securityOrigin, const QString &msg){
        Q_UNUSED(securityOrigin);
        ToastMsg::show(this->parent, "", msg);
    }

    QWebEnginePage* BugsWebPage::createWindow(QWebEnginePage::WebWindowType type){
        Q_UNUSED(type);
        BugsWebPage *bugsWebPage = new BugsWebPage(this->parent);
        bugsWebPage->prev_webPage = this;

        BugsWebView *tmp_view = qobject_cast<BugsWebView*>(this->view());
        tmp_view->setPage(bugsWebPage);
        return bugsWebPage;
    }

    void BugsWebPage::slot_windowCloseRequested(){
        if(this->prev_webPage != nullptr){
            BugsWebView *tmp_view = qobject_cast<BugsWebView*>(this->view());
            tmp_view->setPage(this->prev_webPage);

        }
    }

}

namespace bugs {
    BugsWebView::BugsWebView(QWidget *parent) : QWebEngineView(parent){
        this->parent = parent;
        this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, true);
        this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptCanOpenWindows, true);
    }

    void BugsWebView::setPage(QWebEnginePage *p_page){
        this->window()->hide();

        QWebEngineView::setPage(p_page);
        this->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
        this->setFocus(Qt::FocusReason::PopupFocusReason);
        this->setFocus(Qt::FocusReason::OtherFocusReason);
        this->show();
        this->window()->show();
    }

}


namespace bugs {

    /**
     * @brief 생성자
     * @param parent
     */
    DialogLogin_forBugs::DialogLogin_forBugs(QWidget *parent) : QDialog(parent){
        setInit();
        setUIControl();
        setUIControl_topTitle();
        setUIControl_body();
    }


    /**
     * @brief 다이얼로그 관련 초기설정
     */
    void DialogLogin_forBugs::setInit(){
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
    void DialogLogin_forBugs::setUIControl(){

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
    void DialogLogin_forBugs::setUIControl_topTitle(){

        this->btn_back = GSCommon::getUIBtnImg("btn_back", ":/images/icon_back_g.png");
        this->btn_back->setFixedSize(80,70);
        this->btn_back->setCursor(Qt::PointingHandCursor);
        connect(this->btn_back, &QPushButton::clicked, this, &DialogLogin_forBugs::slot_goBack_webView);

        // 타이틀
        QLabel *label_title = new QLabel;
        label_title->setText(tr("Bugs login"));//label_title->setText(tr("Bugs 로그인"));
        label_title->setAlignment(Qt::AlignCenter);
        label_title->setStyleSheet("color:#FFFFFF; font-size:22px; background-color:transparent;");

        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
        btn_close->setFixedSize(QSize(70,70));
        btn_close->setCursor(Qt::PointingHandCursor);
        connect(btn_close, &QPushButton::clicked, this, &DialogLogin_forBugs::reject);

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
    void DialogLogin_forBugs::setUIControl_body(){

        // create webview
        this->bugsWebView = new BugsWebView(this);
//        connect(this->webEngineView, &QWebEngineView::loadFinished, this, &DialogLogin_forBugs::slot_finishLoadingUrl);
        this->bugsWebView->setContentsMargins(0,0,0,0);
        this->vl_total->addWidget(this->bugsWebView, 1);


        BugsWebPage *bugsWebPage = new BugsWebPage(this);
        connect(bugsWebPage, &BugsWebPage::loadFinished, this, &DialogLogin_forBugs::slot_finishLoadingUrl);
        this->bugsWebView->setPage(bugsWebPage);

        // Request - Bugs API Get
        //   - Authorization Code 발급
        QUrlQuery urlQuery;
        urlQuery.addQueryItem("response_type", "code");
        urlQuery.addQueryItem("client_id", global.bugs_client_id);
        urlQuery.addQueryItem("redirect_uri", global.bugs_auth_res_redirectUrl);

        QUrl url_login( QString("%1/oauth/2/member/login").arg(global.bugs_auth_url) );
        url_login.setQuery(urlQuery.query(QUrl::ComponentFormattingOption::FullyEncoded));

        bugsWebPage->load(url_login);


    }


    /**
     * @brief DialogLogin_forBugs::slot_finishLoadingUrl
     * @param flagSuccess
     */
    void DialogLogin_forBugs::slot_finishLoadingUrl(bool flagSuccess){
        Q_UNUSED(flagSuccess);

        this->applyStyle_backBtn();


        // 응답 확인
        if(this->bugsWebView->page()->url().host() == "beta.roseaudio.kr"){
            if(this->bugsWebView->page()->url().path() == "/service/application/login"){
                this->bugsWebView->page()->runJavaScript("document.body.textContent", [this](const QVariant &result){
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
    void DialogLogin_forBugs::slot_goBack_webView(){
        if(this->bugsWebView->history()->canGoBack()){
            this->bugsWebView->back();
        }
    }


    /**
     * @brief DialogLogin_forBugs::checkResponce_login
     * @param jsonObj_result
     */
    void DialogLogin_forBugs::checkResponse_login(const QJsonObject& jsonObj_result){
        QString code = ProcJsonEasy::getString(jsonObj_result, "code");
        QString error = ProcJsonEasy::getString(jsonObj_result, "error");
        if(!error.isEmpty()){
            this->bugsWebView->back();
            ToastMsg::show(this, "", ProcJsonEasy::getString(jsonObj_result, "error_description"));
        }
        else{
            // 로그인 성공
            global.user_forBugs.set_bugs_login_authCode(code);
            emit this->signal_successLogin();
            this->close();

//            // 벅스 OAuth 2.0 API - 로그인의 경우임
//            QUrlQuery urlQuery;
//            urlQuery.addQueryItem("grant_type", "authorization_code");      // authorization_code를 전달해주세요. 비로그인 상태의 경우 client_credentials를 사용해주세요.
//            urlQuery.addQueryItem("client_secret", "glu!LK5d");
//            urlQuery.addQueryItem("code", code);
//            QByteArray byteArr_param = urlQuery.query(QUrl::ComponentFormattingOption::FullyEncoded).toUtf8();

//            QUrl url_reqToken("https://qa-secure.bugs.co.kr/oauth/citech/token");

//            QWebEngineHttpRequest request;
//            request.setUrl(url_reqToken);
//            request.setPostData(byteArr_param);



        }
    }


    /**
     * @brief DialogLogin_forBugs::applyStyle_backBtn
     */
    void DialogLogin_forBugs::applyStyle_backBtn(){
        if(this->bugsWebView->history()->canGoBack()){
            GSCommon::applyBtnImage(this->btn_back, ":/images/icon_back_w.png");
        }
        else{
            GSCommon::applyBtnImage(this->btn_back, ":/images/icon_back_g.png");
        }

    }

}
