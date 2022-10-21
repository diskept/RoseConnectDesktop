#ifndef SETTINGLIB_H
#define SETTINGLIB_H

#include "common/linker.h"

#include "data/data_media_library.h"

#include <QList>
#include <QLabel>
#include <QWidget>

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>


/**
 * @brief The SettingAppMenu class 설정 > 미디어 라이브러리 @n
 *      미디어 라이브러리 설정 (DB Get, DB Init, DB Add,Del) 지원, 미디어 라이브러리 항목별 DB 스캔, 삭제 지원
 */
class SettingLib : public QWidget
{
    Q_OBJECT
public:
    explicit SettingLib(QWidget *parent = nullptr);
    ~SettingLib();

    void requestMediaLibrary();
    void resizeEvent(QResizeEvent *event) override;                         // filter Box 사이즈 조절을 위해

signals:
    void changedSubMenu(QJsonObject);

private slots:
    void slot_clickedBtn();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedMediaLibraryMore();

private:
    void setInit();
    void setUIControl();
    void clearMediaList();
    void showDlgOfMediaRemove(const int &p_index);
    void showDlgOfDBInit();
    void showDlgOfDbReReceive();
    QWidget* getUIControlOption(int index, QString p_title, QString p_btn, bool flagIsImageBtn=false);
    QWidget* getUIControlMediaLibrary(const int &p_index);
    QString getMedaiNamePath(bool p_flagIsServer, QString p_name, QString p_path);

    void requestDBInit();
    void requestMediaAdd_Del(const int &p_index, bool flagDel=true);
    void setResultOfMediaLibrary(const QJsonObject &p_jsonObject);

private:
    Linker *linker;
    QLabel *lb_path_tmp[9999];
    QList<Data_Media_Library*> *listMedia;
    QVBoxLayout *box_mediaLibrary;

    QLabel *lb_path[9999];
};

#endif // SETTINGLIB_H
