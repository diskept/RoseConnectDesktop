#ifndef ITEMVIDEO_H
#define ITEMVIDEO_H

#include "tidal/AbstractItem.h"
#include "tidal/tidal_struct.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal 전용으로 사용하는 Video 아이템 UI
     */
    class ItemVideo : public AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemVideo(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const tidal::VideoItemData &p_data_video);
        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        tidal::VideoItemData data_video;

        // ui
        QLabel *label_title;
        QLabel *label_creator;

    };

};


#endif // ITEMVIDEO_H
