#ifndef ITEMEXPLORE_H
#define ITEMEXPLORE_H

#include "common/filedownloader.h"
#include "common/linker.h"

#include <QLabel>
#include <QJsonObject>
#include <QPushButton>
#include <QWidget>



namespace roseRadio {

    /**
     * @brief Qobuz 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemExplore : public QPushButton
    {
        Q_OBJECT

    public:
        enum ClickMode_radio{
            AllBox = 1              ///< Hover 영역 전체 박스를 클릭
            , MoreBtn               ///< 더보기 버튼 클릭
            , FavBtn                ///< 좋아요(하트) 버튼 클릭
            , AddCollectionBtn      ///< My Collection 추가 버튼 클릭
            , MusicVideoBtn         ///< Bugs Music Video 버튼 클릭
        };

    public:
        explicit ItemExplore(int index, int section, QWidget *parent = nullptr);
        ~ItemExplore();

        void setData(const QJsonObject&);

    signals:
        void explore_clicked(const int section, const int index, const roseRadio::ItemExplore::ClickMode_radio clickMode);

    protected:
        void enterEvent(QEvent *event) override;
        void leaveEvent(QEvent *event) override;

    private slots:
        void slot_clickBtn_signal();
        void slot_fileDownload_loadImage();

    private:
        void setUIControl();

        void setImage(QString imagePath);
        void paint_image(QPixmap &pixmap);

    private:
        FileDownloader *fileDownLoader;
        QPixmap *pixmap_albumImg;
        QPixmap *pixmap_albumImg_default;

        QLabel *label_background;
        QLabel *label_icon;
        QLabel *label_name;

        QPushButton *btn_allBox;

        int windex = 0;
        int wsection = 0;

        // data
        QJsonObject data_genre;
    };

};

#endif // ITEMEXPLORE_H
