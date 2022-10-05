#ifndef SETTINGTIMER_H
#define SETTINGTIMER_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>


#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief  설정 > 국가 채널 옵션
 */
class SettingTimer : public QWidget
{
    Q_OBJECT
public:
    explicit SettingTimer(QWidget *parent = nullptr);
    void getSettingInfo();

    void requestGetValueTimer();
signals:

private slots:
    void slot_changedIndexTimer(int index);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:
    QComboBox *comboBox_timer;
    QLabel *lb_timer_res;
    void setUIControl();
    QWidget* getUIControlOption(QString p_title);

    QList<int> list_keyValue;

};

#endif // SETTINGTIMER_H
