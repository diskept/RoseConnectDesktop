#ifndef ITEMRECTANGLE_H
#define ITEMRECTANGLE_H

#include "roseRadio/ItemImage_RoseRadio.h"

#include "common/linker.h"

#include "tidal/AbstractItem.h"

#include <QLabel>
#include <QJsonObject>
#include <QPushButton>
#include <QWidget>


namespace roseRadio {

    /**
     * @brief Rose Radio 전용으로 사용하는 Rectangle 아이템 UI.
     */
    class ItemRectangle : public QWidget
    {
        Q_OBJECT

    public:
        explicit ItemRectangle(int index, int section, tidal::AbstractItem::ImageSizeMode imageSizeMode, QWidget *parent = nullptr);
        ~ItemRectangle();

        void setData(const QJsonObject&);
        void setFavorite(const bool flag);

    signals:
        void rectangle_clicked(const int section, const int index, const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio);

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

        QPushButton *btn_reservation;
        QPushButton *btn_favorite;

        // data
        QJsonObject data_channel;

        bool flag_fav = false;

        int windex = 0;
        int wsection = 0;
    };
};
#endif // ITEMRECTANGLE_H
