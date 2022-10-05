#ifndef ITEMTRACKSQURE_BUGS_H
#define ITEMTRACKSQURE_BUGS_H

#include "bugs/bugs_struct.h"

#include "tidal/AbstractItem.h"


namespace bugs {

    /**
     * @brief Bugs 전용으로 사용하는 Track 아이템 UI. like Album 처럼 UI 디자인 비슷한 구조임. (List 형식 아님)
     */
    class ItemTrackSqure_bugs : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemTrackSqure_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const bugs::TrackItemData &p_data_album);                               ///< 실제 사용함. 타이달에서만. struct 사용.
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        bugs::TrackItemData data_track;

        // ui
        QLabel *label_title;
        QLabel *label_mime;
        QLabel *label_artist;
    };

};

#endif // ITEMTRACKSQURE_BUGS_H
