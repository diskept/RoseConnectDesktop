#ifndef DIALOGLOGIN_FORTIDAL_H
#define DIALOGLOGIN_FORTIDAL_H

//#include "common/linker.h"
#include "widget/onoffwidget.h"
#include "common/linker.h"
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QJsonObject>


#include <QLineEdit>

// oAuth 방식으로 변경
#include <QWebEngineView>
#include <QWebEnginePage>
#include <qwebview_global.h>
#include <QPushButton>

namespace tidal {

    /**
     * @brief 타이달 로그인 웹뷰 변경으로 추가@n
     *      Tidal 아이디, Facebook,  로그인 지원
     */
    class TidalWebPage : public QWebEnginePage
    {
        Q_OBJECT
    public:
        TidalWebPage(QWidget *parent = nullptr);
    protected:
        void javaScriptAlert(const QUrl &securityOrigin, const QString &msg) override;
        QWebEnginePage* createWindow(QWebEnginePage::WebWindowType type) override;
    private slots:
        void slot_windowCloseRequested();
    private:
        QWidget *parent;
        TidalWebPage *prev_webPage = nullptr;
    };


    class TidalWebView : public QWebEngineView
    {
        Q_OBJECT
    public:
        TidalWebView(QWidget *parent = nullptr);
        void setPage(QWebEnginePage *p_page);
    private:
        QWidget *parent;
    };

    /**
     * @brief 타이달 서비스에 로그인하기 위한 다이얼로그
     * @author sunnyfish
     */
    class DialogLogin_forTidal : public QDialog
    {
        Q_OBJECT
    public:
        explicit DialogLogin_forTidal(QWidget *parent = nullptr);

    signals:
        //void successLogin();
        void signal_successLogin();

    // 웹로그인 변경으로 추가
    private slots:
        void slot_goBack_webView();
        void slot_finishLoadingUrl(bool);

    private:
        Linker *linker;

        const int DLG_WIDTH = 770;
        const int DLG_HEIGHT= 830;

        // 초기 UI화면 구성
        void setInit();
        void setUIControl();
        void setUIControl_topTitle();
        void setUIControl_body();
        void domainShow(const QVariant &value);

        void applyStyle_backBtn();
        void checkResponse_login(const QJsonObject&);

        // HTTP 요청 및 응답처리
        bool checkUserInput();


        // UI 구성
        QVBoxLayout *vl_total;      ///< 화면 UI구성 전체 마지막 주관하는 Layout
        TidalWebView *tidalWebView;
//        BugsWebPage *bugsWebPage;
        QPushButton *btn_back;

        bool flag_loadUrl = false;

        //const int DLG_WIDTH = 700;
        //const int DLG_HEIGHT= 450;

        // 초기 UI화면 구성
        //void setInit();
        //void setUIControl();
        //void setUIControl_topTitle();
        //void setUIControl_body();

        // HTTP 요청 및 응답처리
        //bool checkUserInput();


        // UI 구성
        //QVBoxLayout *vl_total;      ///< 화면 UI구성 전체 마지막 주관하는 Layout
        //QLineEdit *lineEdit_email;
        //QLineEdit *lineEdit_password;
        //OnOffWidget *onOff;


    //private slots:
        //void slot_clickedLogin();
        //void slot_failedLogin(const QString& errorMsg);
        //void slot_successLogin();

    };

};

#endif // DIALOGLOGIN_FORTIDAL_H
