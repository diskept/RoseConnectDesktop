#ifndef TIDALMAIN_TEST_H
#define TIDALMAIN_TEST_H
#include <home/abstractmaincontent.h>
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>

class TidalMain_Test : public AbstractMainContent
{
    Q_OBJECT
public:
    explicit TidalMain_Test(QWidget *parent = nullptr);

signals:

private slots:

protected:
    void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

private:

    // 메뉴 관련 페이지  ----------------------


    void setUIControl();
    void setUIControl_testStretch();

};

#endif // TIDALMAIN_TEST_H
