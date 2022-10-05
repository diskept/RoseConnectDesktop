#ifndef IMAGELABELFORRADIO_H
#define IMAGELABELFORRADIO_H

#include <common/gscommon.h>
#include <common/filedownloader.h>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QList>

/**
 * @brief 이미지 라벨 라디오 @n
 *      라디오에서 사용하는 이미지 라벨로 hover 기능 제공 @n
 *      @sa ImageLabelRadio::Mode 에 따라 hover 기능 다름
 *
 */
class ImageLabelRadio : public QWidget
{
    Q_OBJECT

public:
    enum Mode{
        Normal,
        User,
        Eidt
    };

    explicit ImageLabelRadio(QWidget *parent=nullptr, Mode = Mode::Normal);
    ~ImageLabelRadio();

    void setHover();
    void setBackgroundImg(const int &p_imgNumber);
    void setImageTitle(const QString &p_url);
    void setTitle(const QString &p_title);
    void setFavorStatus(bool flagOn);

protected:
    const int IMG_WIDTH = 200;
    const int IMG_HEIGHT = 200;

    QList<QString> listHoverPath;
    QList<QString> listHoverCode;

    QLabel *lb_img;
    QWidget *wg_hover;
    bool flagUseHover = false;

private:

    Mode mode;
    bool flagFavorOn = true;

    QLabel *lb_imgTitle;
    QLabel *lb_title;

    QVBoxLayout *boxLayout;
    FileDownloader *fileDownLoader;

    void setInit();
    void setUIControl();
    void setHoverUIControl();

signals:
    void clickedHoverItem(QString p_code);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void loadImage();
    void clickedHoverItem();
};

#endif // IMAGELABELFORRADIO_H
