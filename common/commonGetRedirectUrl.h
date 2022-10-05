#ifndef COMMONGETREDIRECTURL_H
#define COMMONGETREDIRECTURL_H

#include "common/linker.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <qwebview_global.h>
#include <QPushButton>


class CommonWebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    CommonWebPage(QWidget *parent = nullptr);

protected:
    void javaScriptAlert(const QUrl &securityOrigin, const QString &msg) override;
    QWebEnginePage* createWindow(QWebEnginePage::WebWindowType type) override;

private slots:
    void slot_windowCloseRequested();

private:
    QWidget *parent;
    CommonWebPage *prev_webPage = nullptr;
};


class CommonWebView : public QWebEngineView
{
    Q_OBJECT
public:
    CommonWebView(QWidget *parent = nullptr);
    void setPage(QWebEnginePage *p_page);

private:
    QWidget *parent;
};


class CommonGetRedirectUrl : public QWidget
{
    Q_OBJECT
public:
    explicit CommonGetRedirectUrl(const QString type, const QString url, QWidget *parent = nullptr);
    ~CommonGetRedirectUrl();

signals:
    void signal_redirectUrl(const QString url);

private slots:
    void slot_finishLoadingUrl(bool);

private:
    Linker *linker;

    CommonWebView *commonWebView;

    QString reqType = "";
    QString reqUrl = "";
};

#endif // COMMONGETREDIRECTURL_H
