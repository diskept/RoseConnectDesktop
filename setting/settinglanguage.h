#ifndef SETTINGLANGUAGE_H
#define SETTINGLANGUAGE_H


#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QProcess>//cheon07_language

#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief  설정 > 국가 채널 옵션
 */

class SettingLanguage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingLanguage(QWidget *parent = nullptr);
    void getSettingInfo();

    void requestGetValueLanguage();
signals:

private slots:
    void slot_changedIndexLanguage(int index);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:
    QComboBox *comboBox_language;
    QLabel *lb_language_res;


    QProcess *proc = new QProcess(this);//cheon07_language
    QList<int> list_keyValue;


    //void changedLanguage_dialog();
    void setUIControl();
    QWidget* getUIControlOption(QString p_title);

};


#endif // SETTINGLANGUAGE_H

