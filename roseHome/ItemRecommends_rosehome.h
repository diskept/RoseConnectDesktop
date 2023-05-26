#ifndef ITEMRECOMMENDS_ROSEHOME_H
#define ITEMRECOMMENDS_ROSEHOME_H

#include <QWidget>
#include "rosehome_struct.h"
#include "tidal/AbstractItem.h"


namespace roseHome {

    /**
     * @brief Qobuz 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemRecommends_rosehome : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemRecommends_rosehome(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);
        void setData(const roseHome::RecommendsItemData &p_data);

        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        roseHome::RecommendsItemData data_recommends;

        // ui
        QWidget *label_base;
        QLabel *label_title;
        QLabel *label_owner;
        QLabel *image_thumb;
        QLabel *label_thumb;
    };

};
#endif // ITEMRECOMMENDS_ROSEHOME_H
