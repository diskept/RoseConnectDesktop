#ifndef SETTINGFRIENDPAGE_H
#define SETTINGFRIENDPAGE_H

#include "friendprofileitem.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QJsonObject>
#include <QLineEdit>
#include <QScrollArea>

#include <home/abstractpage.h>
#include <home/topmenubar.h>
/**
 * @brief  설정 > 친구관리
 */
class SettingFriendPage : public AbstractPage
{
    Q_OBJECT
public:
    explicit SettingFriendPage(QWidget *parent = nullptr);
    void setDataABSPage(QJsonObject p_data) override;

signals:

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_changedSubTabUI(const QJsonObject &p_data) override;
    void slot_removeRow();
    void slot_searchFriend();

private:

    int next = 0;//cheon211018
    int tmp_totalCount = 0;//cheon211018
    int totalCount = 0;//cheon211018
    TopMenuBar *menubar;
    QStackedWidget *stackedwidget;
    QVBoxLayout *vl_friend;     ///< 친구목록 VL
    QVBoxLayout *vl_receive;    ///< 받은요청 VL
    QVBoxLayout *vl_send;       ///< 보낸요청 VL
    QVBoxLayout *vl_find;       ///< 친구찾기 VL
    QLineEdit *le_search;
    QScrollArea *scrollArea;

    virtual QJsonObject getDataJson() override;
    void setUIControl();
    void requestFriendList();
    void requestReceivedRequest();
    void requestReceivedSend();
    void requestReceivedFind();
    void changedOnlyTabUI_notSendSignal(QString p_step);
    QWidget* getUIScrollViewWidget(QWidget *p_widget);
};

#endif // SETTINGFRIENDPAGE_H
