#ifndef ITEMMYALBUM_BUGS_H
#define ITEMMYALBUM_BUGS_H

#include "bugs/bugs_struct.h"

#include "tidal/AbstractItem.h"

namespace bugs {

    /**
     * @brief Bugs 전용으로 사용하는 MyAlbum 아이템 UI.
     */
    class ItemMyAlbum_bugs : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemMyAlbum_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const bugs::MyAlbumItemData &p_data_my_album);                               ///< 실제 사용함. 타이달에서만. struct 사용.
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        bugs::MyAlbumItemData data_my_album;

        // ui
        QLabel *label_title;
        QLabel *label_track_count;
    };

};


#endif // ITEMMYALBUM_BUGS_H
