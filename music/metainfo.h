#ifndef METAINFO_H
#define METAINFO_H

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include "widget/optionpopup.h"

#include <common/filedownloader.h>

/**
 * @brief 음악- 트랙정보 @n
 *      트랙의 앨범명, 타이틀, 아티스트, 장르, 작곡가 확인
 */
class MetaInfo : public QWidget
{
    Q_OBJECT
public:
    explicit MetaInfo(QWidget *parent = nullptr);
    ~MetaInfo();

    void setData(QJsonObject p_data);

signals:

private slots:
    void slot_loadImage();
    void slot_changedModeEdit();
    void slot_changedModeView();
    void slot_showPopupChangeImg();
    void slot_saveInfo();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private :
    // >>>>>>>>>> DATA >>>>>>>>>>>
    OptionPopup::TypeMenu type_optionMenu;  ///< 옵션팝업을 어느곳에서 띄우는지(부모가 누군지) 유형
    QString albumName;
    QString albumImg;
    QString title;
    QString artist;
    QString genre;
    QString composer;
    QString audio_id;
    // <<<<<<<<<< DATA <<<<<<<<<<<

    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QLabel *lb_albumImg;
    QWidget *widget_albumImgEdit;

    QLineEdit *le_albumName;
    QLineEdit *le_title;
    QLineEdit *le_artist;
    QLineEdit *le_gerne;
    QLineEdit *le_composer;
    QPushButton *btn_edit;
    QPushButton *btn_save;
    QPushButton *btn_cancel;
    QHBoxLayout *hl_btnEdit;

    void setUIControl();
    void requestDataFromAPI();

};

#endif // METAINFO_H
