#ifndef SETTINGINOUTSUBSETTING_H
#define SETTINGINOUTSUBSETTING_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QProcess>//cheon07_language
//#include <widget/flowlayout.h>
//#include <setting/flowlayout.h>


class settingInoutSubsetting: public QWidget
{
       Q_OBJECT
public:
    explicit settingInoutSubsetting(QWidget *parent = nullptr);
    void getSettingInfo();

    void requestGetValueLanguage();

signals:

private slots:
    void slot_changedIndexLanguage(int index);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:
    QComboBox *comboBox_language;
    QLabel *lb_language_res;

    QWidget* getUIControlOption(QString p_title);
    QProcess *proc = new QProcess(this);//cheon07_language
    QList<int> list_keyValue;
    void setUIControl();



};

#endif // SETTINGINOUTSUBSETTING_H

