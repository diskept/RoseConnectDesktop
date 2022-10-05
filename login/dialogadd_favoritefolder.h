#ifndef DIALOGADD_FAVORITEFOLDER_H
#define DIALOGADD_FAVORITEFOLDER_H

#include "common/linker.h"
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>

namespace Dialog{

/**
 * @brief 즐겨찾기 폴더 추가 dlg
 */
class DialogAdd_FavoriteFolder : public QDialog
{
    Q_OBJECT
public:
    explicit DialogAdd_FavoriteFolder(QWidget *parent = nullptr);
    explicit DialogAdd_FavoriteFolder(QString p_dirName, QString p_path, QWidget *parent = nullptr);

    void setFavoriteData(QString p_dirName, QString p_path);

private slots:
    void slot_ok();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private :

    const int DLG_WIDTH = 600;
    const int HTTP_ADD_FAVORITE_FOLDER = 99;

    Linker *linker;
    QLabel *lb_favorPath;
    QLineEdit *lineEdit_name;

    void setInit();
    void setUIControl();
    void requestAddFavoriteFolder();
    bool checkUserInput();
    void setResultOfFavoriteFolder(const QJsonObject &p_jsonData);

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    };
};
};

#endif // DIALOGADD_FAVORITEFOLDER_H
