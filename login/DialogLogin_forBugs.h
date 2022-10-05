#ifndef DIALOGLOGIN_FORBUGS_H
#define DIALOGLOGIN_FORBUGS_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <qwebview_global.h>
#include <QPushButton>

namespace bugs{

    /**
     * @brief 벅스 로그인 관련 웹뷰 @n
     *      벅스 아이디, PAYCO, Facebook 로그인 지원
     */
    class BugsWebPage : public QWebEnginePage
    {
        Q_OBJECT
    public:
        BugsWebPage(QWidget *parent = nullptr);
    protected:
        void javaScriptAlert(const QUrl &securityOrigin, const QString &msg) override;
        QWebEnginePage* createWindow(QWebEnginePage::WebWindowType type) override;
    private slots:
        void slot_windowCloseRequested();
    private:
        QWidget *parent;
        BugsWebPage *prev_webPage = nullptr;
    };


    class BugsWebView : public QWebEngineView
    {
        Q_OBJECT
    public:
        BugsWebView(QWidget *parent = nullptr);
        void setPage(QWebEnginePage *p_page);
    private:
        QWidget *parent;
    };


    /**
     * @brief 벅스 서비스에 로그인하기 위한 다이얼로그
     */
    class DialogLogin_forBugs : public QDialog
    {
        Q_OBJECT
    public:
        explicit DialogLogin_forBugs(QWidget *parent = nullptr);

    signals:
        void signal_successLogin();

    private slots:
        void slot_goBack_webView();
        void slot_finishLoadingUrl(bool);

    private:
        const int DLG_WIDTH = 700;
        const int DLG_HEIGHT= 680;

        // 초기 UI화면 구성
        void setInit();
        void setUIControl();
        void setUIControl_topTitle();
        void setUIControl_body();

        void applyStyle_backBtn();
        void checkResponse_login(const QJsonObject&);

        // HTTP 요청 및 응답처리
        bool checkUserInput();


        // UI 구성
        QVBoxLayout *vl_total;      ///< 화면 UI구성 전체 마지막 주관하는 Layout
        BugsWebView *bugsWebView;
//        BugsWebPage *bugsWebPage;
        QPushButton *btn_back;

    };

};

#endif // DIALOGLOGIN_FORBUGS_H
