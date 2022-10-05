#ifndef ITEMPD_ALBUM_BUGS_H
#define ITEMPD_ALBUM_BUGS_H

#include "bugs/bugs_struct.h"

#include "tidal/AbstractItem.h"


namespace bugs {

    /**
     * @brief Bugs 전용으로 사용하는 PD's Album 아이템 UI.
     */
    class ItemPD_Album_bugs : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemPD_Album_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const bugs::PD_AlbumItemData &p_data_pd_album);                               ///< 실제 사용함. 타이달에서만. struct 사용.
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        bugs::PD_AlbumItemData data_pd_album;

        // ui
        QLabel *label_title;
        QLabel *label_pd_nickname;
    };

};
#endif // ITEMPD_ALBUM_BUGS_H
