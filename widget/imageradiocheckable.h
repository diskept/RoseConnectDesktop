#ifndef IMAGERADIOCHECKABLE_H
#define IMAGERADIOCHECKABLE_H

#include <widget/imagelabelradio.h>
#include <QCheckBox>

/**
 * @brief 이미지 라벨 라디오 @n
 *      라디오 사용자채널 편집에서 사용하는 이미지 라벨로 체크 기능, hover 기능 (삭제 옵션) 제공
 *
 * @note ImageLabelRadio
 */
class ImageRadioCheckable : public ImageLabelRadio
{
    Q_OBJECT
public:
    explicit ImageRadioCheckable(QWidget *parent=nullptr, Mode mode = Mode::Eidt);
    void checked(bool p_flagCheck);
    bool getCheckedValue();
     void setCheckUIControl();

private slots:
     void slot_checkedChanged(int);

private:
    bool flagIsChecked = false;

    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void setCheckedUI();
    void setUnCheckedUI();

    QCheckBox *checkBox;
    QWidget *wgBGSelected;
};

#endif // IMAGERADIOCHECKABLE_H
