#ifndef DIALOGSETTING_OUTPUT_H
#define DIALOGSETTING_OUTPUT_H


#include <QDialog>
#include <QKeyEvent>
#include <QRadioButton>
#include <QWidget>
#include <QComboBox>
#include <widget/onoffwidget.h>
#include <QJsonObject>
#include "widget/dialogconfirm.h"
#include "home/remotebtnitem.h"
#include <QPainter>

namespace  Dialog{

/**
 * @brief 클라우드 타입 선택 Dlg
 *
 * @note 추후 @sa Dialog_Radio 로 대체
 */
class dialogsetting_output : public QDialog
{
    Q_OBJECT
public:
    explicit dialogsetting_output(QWidget *parent = nullptr);
    QString getSelecteddeviceName() const{
        return selected_deviceName;
    };

private slots:
    void slot_clickedOutputChangeSet();
    void slot_clickedOutputsubmqaGain();
    void slot_clickedOutputaddmqaGain();
    void slot_clickedOutputsubcodecTrim();
    void slot_clickedOutputaddcodecTrim();
    void slot_modeOnOff_drc();
    void slot_modeOnOff_32();
    //  void slot_print();
    void slot_changedIndexOutput(int index);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_showDlgOfOutputNotice();
    void slot_clickedOutputmqaGain();
    void slot_clickedOutputcodecTrim();
    void slot_DRC_question_popup();
    void slot_CodecTrim_question_popup();
    void slot_setTextVolumnMin(int p_value);
    void slot_setTextVolumnMax(int p_value);
    void slot_clickedRadioVolLev();

    void slot_clickedPCMquestion();
    void slot_clickedDSDquestion();
    void slot_clickedPreLevelquestion();
    void slot_clickedBalancequestion();
    void slot_clickedHOIquestion();
    void slot_clickedSoftwareVolquestion();
    void slot_setVolumnBalance(int p_value);

private:
    QDialog *loding_ani_dialog = nullptr;
    void loding_ani_init();
    void loding_ani_show();
    void loding_ani_hide();

    QComboBox *comboBoxHdmiDSD;
    QComboBox *comboBoxOutputPreoutLevel;
    QComboBox *comboBoxOutputPCMResampling, *comboBoxOutputPCMMaxSampling;
    //    QComboBox *comboBoxOutputDSD;
    //    QComboBox *comboBoxOutputRoon;
    QRadioButton *radio_vol_sel1, *radio_vol_sel2, *radio_vol_sel3, *radio_vol_sel4;

    OnOffWidget *onOff_DRC, *onOff_32;
    OnOffWidget *onOff[2];
    const int DLG_WIDTH = 800;
    const int DLG_HEIGHT = 560;
    const char* PROPERTY_NAME_READABLE = "readableName";

    QString selected_deviceName;
    QString notice_msg;
    QLabel *lb_notice;
    QWidget *widget_minmaxbox;
    //QWidget *widget_maxbox;
    QWidget *widget_box_notice;
    //QWidget *widget_mqaGain;
    //QWidget *widget_outputGain;
    QPushButton *lb_onOfftitle;
    QPushButton *lb_gain[2];
    RemoteBtnItem *btn_gain_add[2];
    RemoteBtnItem *btn_gain_sub[2];
    QLabel *lb_text[2];
    int outputdBData;
    int mqadBData;
    int radioVolSel;

    QLabel *lb_volumnNumMax;
    QLabel *lb_volumnNumMin;
    QSlider *slider_volMin;
    QSlider *slider_volMax, *slider_volBalance;

    DialogConfirm *dlgConfirmOutput;
    //DialogConfirm;
    QJsonObject OutputDetailData ;

    QRadioButton *radio_Unused;
    QRadioButton *radio_UsbName;

    //  QPushButton *comboBox_1;
    //  QPushButton *comboBox_2;
    //  QPushButton *comboBox_3;
    //  QPushButton *comboBox_4;

    QList<int> list_keyValue;

    void setInit();
    void setUIControl();

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    }

public:
    QPixmap* getcheonUIPixmapImg(QString p_url);
    QPixmap* getcheonUIPixmapImg2(QString p_url);
    QPixmap* getcheonUIPixmapImg_balance(QString p_url);
    QLabel* getcheonUILabelImg(QString p_url);
    QLabel* getcheonUILabelImg2(QString p_url);
    QLabel* getcheonUILabelImg_balance(QString p_url);
    QString get_selected_deviceName();
    void set_selected_deviceName(QString usbstr);
    void setTextName(QString usbstr);
    void setPropertyName(QString usbstr);
    void setDialogOutput_show();
    void setDialogOutput_save(QJsonObject output_rcv);

    QWidget* getBalanceUIControlOption();
    QPixmap drawCMPattern(qreal dpi);
    void setTextVolumn(int p_value);
    QWidget* getVolumeUIControlOptionMin();
    QWidget* getVolumeUIControlOptionMax();
    QWidget* tmp_getVolumeUIControlOption();
    // QWidget* getPlusMinusUIControlOutpution(QString p_title, int i,int data);
    QWidget* getUIControlOption_DRC(QString p_title);
    QWidget* getUIControlOption_32(QString p_title);
    //QWidget* getUIControlOption(QString p_title, int index);
    //  QWidget* getUIControlOutpution(QString p_title, QString p_content, int index);
    //  QWidget* getUIControlOutpution(QString p_title);
    QWidget* getUIControlOutpution(QString p_title, QComboBox *comboBox);
    QWidget* getPlusMinusUIControlOption(QString p_title, int i, int data);
    QJsonObject get_settingOfOutputDetail();
    void set_settingOfOutputDetail(QJsonObject a);
    void getOutputSettingOfsetting();
    void getOutputInOutSettingOfsetting();
    void dlgOutputPass();
    void dlgOutputDRC();
    void dlgOutputMqa();
    void dlgOutputCodecTrim();

};

};


#endif // DIALOGSETTING_OPT_H
