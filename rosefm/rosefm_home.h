#ifndef ROSEFM_HOME_H
#define ROSEFM_HOME_H

#include <widget/flowlayout.h>
#include <common/linker.h>
#include "widget/dialogRoseFmHelp.h"
#include "data/datauser_forRoseFm.h"
#include "rosefm_struct.h"
#include "home/sectionbottom.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QMutex>
#include <QLabel>


QT_BEGIN_NAMESPACE
class QDial;
class QSlider;
QT_END_NAMESPACE

class RoseFmHome : public QWidget
{
  Q_OBJECT
public:
    explicit RoseFmHome(QWidget *parent = nullptr);
    ~RoseFmHome();

    void get_rose_data();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_channel_changed(const QJsonObject &p_jsonObject);

    void slot_btnPower_changed();
    void slot_btnMute_changed();

    void slot_btnCountry_clicked();
    void slot_btnCountry_pressed();
    void slot_btnHelp_changed();

    void slot_btnBack_clicked();
    void slot_btnNext_clicked();

    void slot_btnBack_pressed();
    void slot_btnNext_pressed();

    void slot_btnfre1_clicked();
    void slot_btnfre2_clicked();
    void slot_btnfre3_clicked();
    void slot_btnfre4_clicked();
    void slot_btnfre5_clicked();
    void slot_btnfre6_clicked();

    void slot_btnfre1_pressed();
    void slot_btnfre2_pressed();
    void slot_btnfre3_pressed();
    void slot_btnfre4_pressed();
    void slot_btnfre5_pressed();
    void slot_btnfre6_pressed();

    void slot_volDial_changed(int value);
    void slot_tunDial_changed(int value);
    void slot_tunDial_Autotune(int value);

private:
    void setInit();
    void setUIControl();

    void setLabelText();

    void request_getChannel_info();
    void request_getTune_info();
    void request_favTune_set(int fav_num);
    void request_autTune_set(int ch_num);
    void request_frqTune_set(double frequency);

private:
    QMutex mutex;
    Linker *linker;

    DialogRoseFmHelp *dlgHelp;

    int volum_val = 0;
    int tun_val_before = 0;

    int auto_tune_idx = 0;

    QSlider *fre_slider;

    QLabel *lb_channel;
    QLabel *lb_country;

    QDial *vol_dial;
    QDial *tun_dial;

    QPixmap *vol_pixmap;
    QPixmap tmp_imgvol;

    QPixmap *tun_pixmap;
    QPixmap tmp_imgtun;

    QLabel *lb_vol_dial;
    QLabel *lb_tun_dial;

    QPushButton *btn_tunled;
    QPushButton *btn_stereo;
    QPushButton *btn_country;
    QPushButton *btn_Power;
    QPushButton *btn_Mute;
    QPushButton *btn_Help;
    QPushButton *btn_Next;
    QPushButton *btn_Back;

    QPushButton *btn_favorite_fre1;
    QPushButton *btn_favorite_fre2;
    QPushButton *btn_favorite_fre3;
    QPushButton *btn_favorite_fre4;
    QPushButton *btn_favorite_fre5;
    QPushButton *btn_favorite_fre6;

    QLabel *lb_favorite_fre1;
    QLabel *lb_favorite_fre2;
    QLabel *lb_favorite_fre3;
    QLabel *lb_favorite_fre4;
    QLabel *lb_favorite_fre5;
    QLabel *lb_favorite_fre6;

    QList<roseFM::RosefmCountryInfo> list_countryInfo;
    QList<roseFM::RosefmChannelInfo> list_channelInfo;

    bool flag_power = false;
    bool flag_mute = false;

    bool flag_fre1 = false;
    bool flag_fre2 = false;
    bool flag_fre3 = false;
    bool flag_fre4 = false;
    bool flag_fre5 = false;
    bool flag_fre6 = false;

    bool flag_reload = false;
    double fre_noise_value = 1001;

};
#endif // ROSEFM_HOME_H
