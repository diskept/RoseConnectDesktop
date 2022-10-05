#ifndef ITEMIMAGE_ROSERADIO_H
#define ITEMIMAGE_ROSERADIO_H

#include "common/filedownloader.h"
#include "common/linker.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QJsonObject>
#include <QPushButton>
#include <QWidget>


namespace roseRadio {

    /**
     * @brief Rose Radio 전용으로 사용하는 Image 아이템 UI.
     */
    class ItemImageRoseRadio : public QWidget
    {
        Q_OBJECT

    public:
        enum ClickMode_RoseRadio{
            AllBox = 1              ///< Hover 영역 전체 박스를 클릭
            , PlayBtn               ///< 재생버튼 클릭
            , MoreBtn               ///< 더보기 버튼 클릭
            , FavBtn_Add            ///< 좋아요(하트) 버튼 클릭
            , FavBtn_Delete         ///< 좋아요(하트) 버튼 클릭
        };

    public:
        explicit ItemImageRoseRadio(int imageSize, QWidget *parent = nullptr);
        ~ItemImageRoseRadio();

        void setImage(QString imagePath);
        void setTypeOnAir(QString imagePath);

    signals:
        void roseRadio_clicked(const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio);

    protected:
        void enterEvent(QEvent *event) override;
        void leaveEvent(QEvent *event) override;

    private slots:
        void slot_clickBtn_signal();
        void slot_fileDownload_loadImage();

    private:
        void setUIControl();

        void paint_image(QPixmap &pixmap);

    private:
        FileDownloader *fileDownLoader;
        QPixmap *pixmap_albumImg;
        QPixmap *pixmap_albumImg_default;

        // ui
        QVBoxLayout *vBox_image;

        QLabel *label_image;
        QLabel *label_imageOnAir;

        QPushButton *btn_allBox;
        QPushButton *btn_play;

        int image_size = 0;
    };
};
#endif // ITEMIMAGE_ROSERADIO_H
