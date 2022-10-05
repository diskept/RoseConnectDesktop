#ifndef SETTINGVU_H
#define SETTINGVU_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include <widget/flowlayout.h>
#include <widget/onoffwidget.h>

/**
 * @brief The SettingVU class 설정 > VU 옵션
 */
class SettingVU : public QWidget
{
    Q_OBJECT
public:
    explicit SettingVU(QWidget *parent = nullptr);
    void requestVUData();
signals:

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_changedThema();
    void slot_modeOnOff(bool flagOn);

private:

    bool delay_flag = false;//c220902_1
    int curr_vu_sytle = 0;
    int curr_vu_on_off = 0;

    QPushButton *btn_1;
    QPushButton *btn_2;
    QPushButton *btn_3;
    QPushButton *btn_4;
    QPushButton *btn_5;
    QPushButton *btn_6;
    QPushButton *btn_7;
    QPushButton *btn_8;
    QPushButton *btn_9;
    QPushButton *btn_10;
    QPushButton *btn_11;
    QPushButton *btn_12;
    QPushButton *btn_13;
    QPushButton *btn_14;
    QPushButton *btn_15;

    QLabel *btn_1_lb;
    QLabel *btn_2_lb;
    QLabel *btn_3_lb;
    QLabel *btn_4_lb;
    QLabel *btn_5_lb;
    QLabel *btn_6_lb;
    QLabel *btn_7_lb;
    QLabel *btn_8_lb;
    QLabel *btn_9_lb;
    QLabel *btn_10_lb;
    QLabel *btn_11_lb;
    QLabel *btn_12_lb;
    QLabel *btn_13_lb;
    QLabel *btn_14_lb;
    QLabel *btn_15_lb;


    FlowLayout *fl_vu;
    OnOffWidget *onOff;


    void setUIControl();
    void clearUnSelectMode();
    void setVUMode(int p_vu_mode);
    QPushButton* getUIControlButton(QString text, int vu_mode, QLabel* p_lb);
    QWidget *getUIControlOption(QString p_title);

};

#endif // SETTINGVU_H
