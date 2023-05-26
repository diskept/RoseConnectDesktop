#ifndef ITEMPLAYLIST_ROSEHOME_H
#define ITEMPLAYLIST_ROSEHOME_H

#include "roseHome/rosehome_struct.h"

#include "tidal/AbstractItem.h"

#include <QWidget>


namespace roseHome {

    /**
     * @brief Qobuz 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemPlaylist_rosehome : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemPlaylist_rosehome(int index, int section, ImageSizeMode imageSizeMode, int type, bool flagPlay, QWidget *parent = nullptr);
        ~ItemPlaylist_rosehome();

        void setData(const roseHome::PlaylistItemData &p_data);
        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        roseHome::PlaylistItemData data_playlist;

        // ui
        QLabel *label_title;
        QLabel *label_owner;
        QLabel *image_thumb;
        QLabel *label_share;
        QLabel *label_thumb;

        int playlist_type = 0;
        bool flag_btnPlay = false;
    };

};
#endif // ITEMPLAYLIST_ROSEHOME_H
