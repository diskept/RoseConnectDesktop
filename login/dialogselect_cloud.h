#ifndef DIALOGSELECT_CLOUD_H
#define DIALOGSELECT_CLOUD_H

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
class DialogSelect_Cloud : public QDialog
{
    Q_OBJECT
public:
    explicit DialogSelect_Cloud(QWidget *parent = nullptr);
    QString getSelectedCloudName() const{
        return selectedCloudName;
    };

private slots:
    void slot_clickedAdd();
private:

    const int DLG_WIDTH = 600;
    const int DLG_HEIGHT = 380;
    const char* PROPERTY_NAME_READABLE = "readableName";

    QString selectedCloudName;

    QRadioButton *radio_ftp;
    QRadioButton *radio_webdav;

    void setInit();
    void setUIControl();

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    }
};

};

#endif // DIALOGSELECT_CLOUD_H
