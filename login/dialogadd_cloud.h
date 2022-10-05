#ifndef DIALOGADD_CLOUD_H
#define DIALOGADD_CLOUD_H

#include <QDialog>
#include <QLineEdit>

namespace Dialog {

    enum DialogType{
        ADD_FTP,
        ADD_WEBDAV
    };

/**
* @brief 클라우드 추가 Dlg @n
*       @sa Dialog::DialogType enum을 이용해 FTP, WEBDAV 구분
*/
class DialogAdd_Cloud : public QDialog
{
    Q_OBJECT

public:

    explicit DialogAdd_Cloud(DialogType dlgType, QWidget *parent = nullptr);

private slots:
    void slot_clickedOk();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:

    const int DLG_WIDTH = 600;
    const int DLG_HEIGHT = 490;

    const int HTTP_NETWORK_ADD_CLOUD = 99;

    DialogType dlgType;

    QLineEdit *edit_name;
    QLineEdit *edit_host;
    QLineEdit *edit_port;
    QLineEdit *edit_userName;
    QLineEdit *edit_pw;

    void setInit();
    void setUIControl();
    bool checkUserInput();
    void requestAddCloud();
    void setResultOfAddCloud(const QJsonObject &p_json);

};
};

#endif // DIALOGADD_CLOUD_H
