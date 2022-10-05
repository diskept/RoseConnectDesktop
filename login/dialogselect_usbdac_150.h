#ifndef DIALOGSELECT_USBDAC_150_H
#define DIALOGSELECT_USBDAC_150_H

#include <QDialog>
#include <QKeyEvent>
#include <QRadioButton>
#include <QWidget>

namespace  Dialog{

/**
 * @brief 클라우드 타입 선택 Dlg
 *
 * @note 추후 @sa Dialog_Radio 로 대체
 */
class DialogSelect_Usbdac_150 : public QDialog
{
    Q_OBJECT
public:
    explicit DialogSelect_Usbdac_150(QWidget *parent = nullptr);
    QString getSelectedUsbName() const{
        return selectedUsbName;
    };

private slots:
    void slot_clickedAdd();
private:

    const int DLG_WIDTH = 600;
    const int DLG_HEIGHT = 380;
    const char* PROPERTY_NAME_READABLE = "readableName";

    QString selectedUsbName;

    QRadioButton *radio_Unused;
    QRadioButton *radio_UsbName;

    void setInit();
    void setUIControl();

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    }

public:
    QString get_selectedUsbName();
    void set_selectedUsbName(QString usbstr);
    void setTextName(QString usbstr);
    void setPropertyName(QString usbstr);
};

};

#endif // DIALOGSELECT_CLOUD_H
