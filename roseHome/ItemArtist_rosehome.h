#ifndef ITEMARTIST_ROSEHOME_H
#define ITEMARTIST_ROSEHOME_H

#include "roseHome/rosehome_struct.h"

#include "tidal/AbstractItem.h"     // TODO :: Added Jeon 16/12/2020


namespace roseHome {

    /**
     * @brief Qobuz 전용으로 사용하는 Artist 아이템 UI
     */
    class ItemArtist_roseHome : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemArtist_roseHome(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const roseHome::ArtistItemData &p_data_artist);
        int get_fixedHeight() override;

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
