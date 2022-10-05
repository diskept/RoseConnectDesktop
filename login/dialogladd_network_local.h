#ifndef DIALOGLADD_NETWORK_LOCAL_H
#define DIALOGLADD_NETWORK_LOCAL_H

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

namespace Dialog{

    /**
     * @brief ㅽ듃뚰겕 대뜑 異붽 Dlg
     */
    class DialogAdd_Network_local : public QDialog
    {
        Q_OBJECT
    public:
        explicit DialogAdd_Network_local(QWidget *parent = nullptr);
        void setFolderInfo(QString p_host, QString p_id, QString p_pw);

    signals:

    private slots:
        void slot_ok();
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
        void slot_qprocessError(QProcess::ProcessError error);

    private:

        const int DLG_WIDTH = 600;
        const int DLG_HEIGHT= 425;
        const int HTTP_ADD_NETWORK_FOLDER = 99;

        Linker *linker;
        QLineEdit *lineEdit_http;
        QLineEdit *lineEdit_id;
        QLineEdit *lineEdit_pw;

        void setInit();
        void setUIControl();
        void requestAddNetworkFolder(const QString &p_ip, const QString &p_path);
        void setResultOfAddNetworkFolder(const QJsonObject &p_json);
        bool checkUserInput();

        void keyPressEvent(QKeyEvent *event) override{
            if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
                return;
            }
            QDialog::keyPressEvent(event);
        };

    };

};

#endif // DIALOGADD_NETWORK_H
