#ifndef ITEMARTIST_BUGS_H
#define ITEMARTIST_BUGS_H

#include <QWidget>
#include "bugs/bugs_struct.h"
#include "tidal/AbstractItem.h"

namespace bugs {

    /**
     * @brief Bugs 전용으로 사용하는 Artist 아이템 UI.
     */
    class ItemArtist_bugs : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemArtist_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);
        void setData(const bugs::ArtistItemData &p_data_artist);                               ///< 실제 사용함. 타이달에서만. struct 사용.

        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        bugs::ArtistItemData data_artist;

        // ui
        QLabel *label_artist;
    };

};

#endif // ITEMARTIST_BUGS_H
