#ifndef DIALOGPOWERON_H
#define DIALOGPOWERON_H

#include <QDialog>
#include <QLabel>

/**
 * @brief The DialogConfirm class : Confirm Dialog
 * @details Rose Connector 에서는 alert Dialog 없음.
 * @note 확인만 보이는 alert Dialog 도 지원되게 수정하기위해 enum DlgConfirmType 추가함
 */
class DialogPowerOn : public QDialog
{
    Q_OBJECT
public:
    enum DlgPowerOnType {
        OKCancel,       ///< 일반적인 Confirm 창 (확인,취소 버튼 존재. 상단 X 닫기버튼 있음)
        OK              ///< 확인버튼만 존재 (상단 X 닫기버튼도 없음. only 수락만 가능하게끔)
    };
    explicit DialogPowerOn(QWidget *parent = nullptr, DlgPowerOnType p_confirmType=DlgPowerOnType::OKCancel);

    void setTitle(const QString &p_title);
    void setText(const QString &p_text);
    void setAlertMode();

private:
    DlgPowerOnType confirmType;
    QLabel *lb_title;
    QLabel *lb_text;
    QPushButton *btn_cancel;

    void setInit();
    void setUIControl();
};

#endif // DIALOGPOWERON_H
