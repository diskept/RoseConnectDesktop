#ifndef ITEMARTIST_FORAPPLE_H
#define ITEMARTIST_FORAPPLE_H

#include "apple/Apple_struct.h"

#include "tidal/AbstractItem.h"     // TODO :: Added diskept 16/12/2020


namespace apple {

    /**
     * @brief Qobuz 전용으로 사용하는 Artist 아이템 UI
     */
    class ItemArtist_apple : public tidal::AbstractItem
    {
        Q_OBJECT
    public:
        explicit ItemArtist_apple(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        //void setData(const qobuz::ArtistItemData &p_data_artist);                               ///< 실제 사용함. 타이달에서만. struct 사용.
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        //qobuz::ArtistItemData data_artist;

        // ui
        QLabel *label_artist;

    };
};

#endif // ITEMARTIST_FORAPPLE_H
