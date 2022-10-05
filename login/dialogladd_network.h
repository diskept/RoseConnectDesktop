#ifndef DIALOGADD_NETWORK_H
#define DIALOGADD_NETWORK_H

#include "common/linker.h"
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QJsonObject>
#include <QProcess>
#include <QKeyEvent>
#include <QLineEdit>
#include "widget/myqwidget.h"//c220906_1

namespace Dialog{

/**
 * @brief 네트워크 폴더 추가 Dlg
 */
class DialogAdd_Network : public QDialog
{
    Q_OBJECT
public:
    explicit DialogAdd_Network(QWidget *parent = nullptr);
    void setFolderInfo(QString p_host, QString p_id, QString p_pw);

signals:
    void signal_process_smb(QString);//c220909_1
private slots:

    void slot_textChanged(QString str);//c220913_2
    void slot_process_smb(QString);//c220909_1
    void slotShortcutFinderSMB();//c220906_1
    void slot_folderSearch();//c220906_1
    void slot_ok();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_qprocessError(QProcess::ProcessError error);

private:

    QPushButton *btn_search;//c220906_1
    const int DLG_WIDTH = 600;
    const int DLG_HEIGHT= 425;
    const int HTTP_ADD_NETWORK_FOLDER = 99;

    Linker *linker;
    QLineEdit *lineEdit_http;
    QLineEdit *lineEdit_id;
    QLineEdit *lineEdit_pw;
    QLineEdit *lineEdit_path;//c220913_1
    QLineEdit *lineEdit_sudopw;//c220913_1
    QLineEdit *lineEdit_username;//c220913_1

    void setInit();
    void setUIControl();
    void requestAddNetworkFolder(const QString &p_ip, const QString &p_path);
    void setResultOfAddNetworkFolder(const QJsonObject &p_json);
    bool checkUserInput();
    void dialog_comfirmNetworkfolder_forFinder();//c220907_2

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    };

};

};

#endif // DIALOGADD_NETWORK_H
