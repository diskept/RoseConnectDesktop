#ifndef ITEMALBUM_BUGS_H
#define ITEMALBUM_BUGS_H

#include "bugs/bugs_struct.h"

#include "tidal/AbstractItem.h"


namespace bugs {

    /**
     * @brief Bugs 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemAlbum_bugs : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemAlbum_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const bugs::AlbumItemData &p_data_album);                               ///< 실제 사용함. 타이달에서만. struct 사용.
        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        bugs::AlbumItemData data_album;

        // ui
        QLabel *label_title;
        QLabel *label_artist;
    };

};

#endif // ITEMALBUM_BUGS_H
