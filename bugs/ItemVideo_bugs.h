#ifndef ITEMVIDEO_BUGS_H
#define ITEMVIDEO_BUGS_H

#include "bugs/bugs_struct.h"

#include "tidal/AbstractItem.h"


namespace bugs {

    /**
     * @brief Bugs 전용으로 사용하는 Video 아이템 UI.
     */
    class ItemVideo_bugs : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemVideo_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const bugs::VideoItemData &p_data_video);
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        bugs::VideoItemData data_video;

        // ui
        QLabel *label_title;
        QLabel *label_creator;

    };

};

#endif // ITEMVIDEO_BUGS_H
