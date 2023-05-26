#ifndef ITEMHISTORY_ROSEHOME_H
#define ITEMHISTORY_ROSEHOME_H

#include "roseHome/rosehome_struct.h"
#include "tidal/AbstractItem.h"

#include <QWidget>


namespace roseHome {

    /**
     * @brief Qobuz 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemHistory_rosehome : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemHistory_rosehome(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent = nullptr);
        ~ItemHistory_rosehome();

        void setData(const roseHome::HistoryItemData &p_data);
        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        roseHome::HistoryItemData data_history;

        // ui
        QLabel *label_title;

        bool flag_btnPlay = false;
    };

};
#endif // ITEMHISTORY_ROSEHOME_H
