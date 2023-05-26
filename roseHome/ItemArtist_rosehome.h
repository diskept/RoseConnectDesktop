#ifndef ITEMARTIST_ROSEHOME_H
#define ITEMARTIST_ROSEHOME_H

#include "roseHome/rosehome_struct.h"

#include "tidal/AbstractItem.h"     // TODO :: Added diskept 16/12/2020


namespace roseHome {

    /**
     * @brief Qobuz 전용으로 사용하는 Artist 아이템 UI
     */
    class ItemArtist_rosehome : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemArtist_rosehome(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);
        ~ItemArtist_rosehome();

        void setData(const roseHome::ArtistItemData &p_data_artist);
        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        roseHome::ArtistItemData data_artist;

        // ui
        QLabel *label_artist;
        QLabel *label_info;
    };
};


#endif // ITEMARTIST_ROSEHOME_H
