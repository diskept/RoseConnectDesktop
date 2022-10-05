#ifndef ITEMALBUM_ROSEHOME_H
#define ITEMALBUM_ROSEHOME_H

#include "roseHome/rosehome_struct.h"
#include "tidal/AbstractItem.h"

#include <QWidget>


namespace roseHome {

    /**
     * @brief Qobuz 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemAlbum_rosehome : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemAlbum_rosehome(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent = nullptr);

        void setData(const roseHome::AlbumItemData &p_data);
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        roseHome::AlbumItemData data_album;

        // ui
        QLabel *label_title;
        QLabel *label_artist;

        bool flag_btnPlay = false;
    };

};
#endif // ITEMALBUM_ROSEHOME_H
