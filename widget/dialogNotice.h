#ifndef DIALOGNOTICE_H
#define DIALOGNOTICE_H

#include "common/filedownloader.h"

#include "widget/textlonganimation.h"

#include <QDialog>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>


class DialogNotice : public QDialog
{
    Q_OBJECT
public:
    enum DlgNotice {
        Notice_default
        , Music_album_info
        , Tidal_album_credit
        , Tidal_artist_more
        , Bugs_artist_more
        , Qobuz_album_info
        , Qobuz_artist_more
    };
    explicit DialogNotice(QWidget *parent = nullptr, DlgNotice p_confirmType = DlgNotice::Notice_default);
    ~DialogNotice();

    void setData_formJsonObj(const QJsonObject&);

private slots:
    void slot_fileDownload_loadImage();

private:
    void setInit();
    void setUIControl();

    void paint_imageBig(QPixmap &pixmap);

private:
    DlgNotice confirmType;

    QVBoxLayout *box_total;
    QWidget *widget_total;
    QScrollArea *scrollArea_content;

    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QLabel *lb_title_img;

    QLabel *lb_titleType;
    LabelLongAnimation *lb_title;
    QLabel *lb_artist;
    QLabel *lb_composer;
    QLabel *lb_released;

    QLabel *lb_resolution;
    QLabel *lb_content;

    QPushButton *btn_confirm;
};
#endif // DIALOGNOTICE_H
