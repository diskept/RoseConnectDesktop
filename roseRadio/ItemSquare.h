#ifndef ITEMSQUARE_H
#define ITEMSQUARE_H

#include "roseRadio/ItemImage_RoseRadio.h"

#include "common/linker.h"

#include "tidal/AbstractItem.h"

#include <QLabel>
#include <QJsonObject>
#include <QPushButton>
#include <QWidget>


namespace roseRadio {

    /**
     * @brief RoseRadio 전용으로 사용하는 Square 아이템 UI.
     */
    class ItemSquare : public QWidget
    {
        Q_OBJECT

    public:
        enum ClickMode_square{
            AllBox = 1              ///< Hover 영역 전체 박스를 클릭
            , PlayBtn               ///< 재생버튼 클릭
            , MoreBtn               ///< 더보기 버튼 클릭
            , FavBtn_Add            ///< 좋아요(하트) 버튼 클릭
            , FavBtn_Delete         ///< 좋아요(하트) 버튼 클릭
        };

    public:
        explicit ItemSquare(int index, int section, tidal::AbstractItem::ImageSizeMode imageSizeMode, QWidget *parent = nullptr);
        ~ItemSquare();

        void setData(const QJsonObject&);
        void setFavorite(const bool flag);

    signals:
        void square_clicked(const int section, const int index, const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio);

    private slots:
        void slot_clickBtn_signal();
        void slot_clickImg_signal(const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio);

    private:
        void setUIControl();

    private:
        tidal::AbstractItem::ImageSizeMode imageSize;
        roseRadio::ItemImageRoseRadio *itemImage;

        // ui
        QLabel *label_title;
        QLabel *label_name;
        QLabel *label_resolution;

        QPushButton *btn_favorite;

        // data
        QJsonObject data_channel;

        bool flag_fav = false;

        int windex = 0;
        int wsection = 0;
    };
};
#endif // ITEMSQUARE_H
