#ifndef DIALOGWAKEUP_H
#define DIALOGWAKEUP_H

#include "common/linker.h"

#include <QDialog>
#include <QLabel>

/**
 * @brief The DialogWakeUp class : Rose Wake-up Dialog
 * @details Rose Connect 에서는 alert Dialog 없음.
 * @note 확인만 보이는 alert Dialog 도 지원되게 수정하기위해 enum DlgConfirmType 추가함
 */
class DialogWakeUp : public QDialog
{
    Q_OBJECT
public:
    enum DlgWakeUpType {
        OKCancel,       ///< 일반적인 Confirm 창 (확인,취소 버튼 존재. 상단 X 닫기버튼 있음)
        OK              ///< 확인버튼만 존재 (상단 X 닫기버튼도 없음. only 수락만 가능하게끔)
    };
    explicit DialogWakeUp(QWidget *parent = nullptr, DlgWakeUpType p_confirmType = DlgWakeUpType::OKCancel);

    void setTitle(const QString &p_title);
    void setText(const QString &p_text);
    void setbText(const QString &p_text);
    void setDevice(const QString &p_name);
    void setAlertMode();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_power_on();
    void slot_tmpMac_reject();

private:
    DlgWakeUpType confirmType;
    QLabel *lb_title;
    QLabel *lb_text;
    QLabel *lb_img_power;
    QPixmap *pixmap_power;
    QLabel *lb_btext;
    QLabel *lb_device;
    QPushButton *btn_cancel;

    Linker *linker;

    void setInit();
    void setUIControl();
};

#endif // DIALOGWAKEUP_H
