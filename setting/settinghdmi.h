#ifndef SETTINGHDMI_H
#define SETTINGHDMI_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>


#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief  설정 > 국가 채널 옵션
 */
class SettingHDMI : public QWidget
{
    Q_OBJECT
public:
    explicit SettingHDMI(QWidget *parent = nullptr);
    void getSettingInfo();

    void requestGetValueHDMI();
signals:

private slots:
    void slot_changedIndexHDMI(int index);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:
    QComboBox *comboBox_hdmi;
    QLabel *lb_hdmi_res;
    void setUIControl();
    QWidget* getUIControlOption(QString p_title);

    QList<int> list_keyValue;

};

#endif // SETTINGHDMI_H
