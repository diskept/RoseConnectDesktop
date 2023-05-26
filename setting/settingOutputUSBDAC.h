#ifndef SETTINGOUTPUTUSBDAC_H
#define SETTINGOUTPUTUSBDAC_H

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QLabel>
#include <QDebug>
#include <QDialog>
#include <QWidget>
#include <QKeyEvent>
#include <QScrollArea>
#include <QRadioButton>
#include <QJsonDocument>


/**
 * @brief USBDAC List Dialog
 */
class settingOuputUSBDAC : public QDialog
{
    Q_OBJECT
public:
    explicit settingOuputUSBDAC(QJsonObject, QWidget *parent = nullptr);

signals:
    void signal_loading_stateConfig(const int, const QString, const int);

private slots:
    void slot_clickedBtnRadio();
    void slot_clickedApply();

private:
    void setInit();
    void setUIControl();

    void setData_usbDACList();

private:
    QWidget *usbDACList = nullptr;
    QVBoxLayout *vBox_usbDACList = nullptr;
    //QScrollArea *scrollArea = nullptr;

    QRadioButton *btnUSBDAC[255];

    QJsonArray jsonArray_usbDAC;

    QString curr_usbDAC;
    QString radio_style;
    QString usbList_sytle;
};
#endif // SETTINGOUTPUTUSBDAC_H
