#ifndef VOLUMNCONTROL_H
#define VOLUMNCONTROL_H

#include "common/linker.h"

#include <QLabel>
#include <QObject>
#include <QSlider>
#include <QWidget>
#include <QMouseEvent>
#include <QVBoxLayout>


/**
 * @brief The VolumnControl class 볼륨 조절 컨트롤
 */
class VolumnControl : public QWidget
{
    Q_OBJECT
public:
    explicit VolumnControl(QWidget *parent = nullptr);
    ~VolumnControl();

    void setValueVolume(int p_value);

signals:
    void clickedVolumnSlide(int pvalue);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);


    void slot_volumeBtn_Click();            ///< Add, Sub Button Click Slot Func.
    void slot_volumeSlide_change(int);      ///< Slider Click Slot Func.
    void slot_volume_change(int&);          ///< Receive volume Slot Func.
    void slot_slider_auto_change();

private:
    void setUIControl();

    void complete_chagne_volumeLevel(int p_value);

private:
    Linker *linker;

    QTimer *timer;

    QLabel *lb_volumnNum;
    QSlider *slider_vol;

    bool flag_auto_change = false;
};





class VolSlider : public QSlider
{
    Q_OBJECT
public:
    VolSlider(QWidget *parent = nullptr) : QSlider(parent) {}

signals:
    void signal_vol_slider_clicked(int);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override {

        QSlider::mouseReleaseEvent(event);

        int value = this->maximum() - (event->y() / 3);

        if(value >= 99){
            value = 99;
        }
        else if(value <= 0){
            value = 0;
        }

        emit signal_vol_slider_clicked(value);
    };
};
#endif // VOLUMNCONTROL_H
