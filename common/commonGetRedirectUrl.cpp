#include "common/commonGetRedirectUrl.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/my_lang.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/sqlitehelper.h"

#include "widget/toastmsg.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QFocusEvent>
#include <QPropertyAnimation>
#include <QUrlQuery>
#include <QWebEngineHistory>
#include <QWebEngineSettings>
#include <QWebEngineProfile>


CommonWebPage::CommonWebPage(QWidget *parent) : QWebEnginePage(parent){

    this->parent = parent;
    this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, true);
    this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptCanOpenWindows, true);
    this->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    connect(this, &CommonWebPage::windowCloseRequested, this, &CommonWebPage::slot_windowCloseRequested);
}

void CommonWebPage::javaScriptAlert(const QUrl &securityOrigin, const QString &msg){

    Q_UNUSED(securityOrigin);
    ToastMsg::show(this->parent, "", msg);
}

QWebEnginePage* CommonWebPage::createWindow(QWebEnginePage::WebWindowType type){

    Q_UNUSED(type);
    CommonWebPage *commonWebPage = new CommonWebPage(this->parent);
    commonWebPage->prev_webPage = this;

    CommonWebView *tmp_view = qobject_cast<CommonWebView*>(this->view());
    tmp_view->setPage(commonWebPage);

    return commonWebPage;
}

void CommonWebPage::slot_windowCloseRequested(){

    if(this->prev_webPage != nullptr){
        CommonWebView *tmp_view = qobject_cast<CommonWebView*>(this->view());
        tmp_view->setPage(this->prev_webPage);
    }
}




CommonWebView::CommonWebView(QWidget *parent) : QWebEngineView(parent){

    this->parent = parent;
    this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, true);
    this->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptCanOpenWindows, true);
}

void CommonWebView::setPage(QWebEnginePage *p_page){

    this->window()->hide();

    QWebEngineView::setPage(p_page);
    this->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    this->setFocus(Qt::FocusReason::PopupFocusReason);
    this->setFocus(Qt::FocusReason::OtherFocusReason);
    this->show();
    //this->window()->show();
}





CommonGetRedirectUrl::CommonGetRedirectUrl(const QString type, const QString url, QWidget *parent) : QWidget(parent) {

    linker = Linker::getInstance();

    this->reqType = type;
    this->reqUrl = url;

    // create webview
    this->commonWebView = new CommonWebView(this);
    this->commonWebView->setContentsMargins(0 ,0, 0, 0);
    this->commonWebView->settings();
    this->commonWebView->hide();

    CommonWebPage *commonWebPage = new CommonWebPage(this);
    connect(commonWebPage, &CommonWebPage::loadFinished, this, &CommonGetRedirectUrl::slot_finishLoadingUrl);

    this->commonWebView->setPage(commonWebPage);

    this->commonWebView->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
    this->commonWebView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    commonWebPage->load(QUrl(this->reqUrl));
}

CommonGetRedirectUrl::~CommonGetRedirectUrl(){

    this->deleteLater();
}

void CommonGetRedirectUrl::slot_finishLoadingUrl(bool flagSuccess){

    Q_UNUSED(flagSuccess);

    //qDebug() << this->commonWebView->page()->url().host() << "\n";
    //qDebug() << this->commonWebView->page()->url().path() << "\n";
    qDebug() << this->commonWebView->page()->url().toString();

    if(this->reqType == "ROSE"){
        if(this->commonWebView->page()->url().host() == "link.roseaudio.kr" || this->commonWebView->page()->url().host() == "dev.link.roseaudio.kr"){
            emit this->signal_redirectUrl(this->commonWebView->page()->url().toString());
        }
    }
    else if(this->reqType == "CD"){
        emit this->signal_redirectUrl(this->commonWebView->page()->url().toString());
    }
}
