#ifndef DIALOGEDITRADIO_H
#define DIALOGEDITRADIO_H

#include <QDialog>
#include <QLineEdit>
#include <QKeyEvent>
#include "common/linker.h"

namespace Dialog{
/**
 * @brief The DialogEditRadio class : 방송 채널 추가 Dlg .h
 */
class DialogEditRadio : public QDialog
{
    Q_OBJECT
public:
    explicit DialogEditRadio(QWidget *parent = nullptr);


private:

    const int DLG_WIDTH = 600;
    const int DLG_HEIGHT = 360;
    const int HTTP_ADD_CHANNEL = 99;

    Linker *linker;
    QLineEdit *le_name;
    QLineEdit *le_url;

    void setInit();
    void setUIControl();
    bool checkUserInput();
    void requestAddChannel();
    void setResultOfAddChannel(const QJsonObject&);

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    };

private slots:
    void slot_clickedSave();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

};
};

#endif // DIALOGEDITRADIO_H
