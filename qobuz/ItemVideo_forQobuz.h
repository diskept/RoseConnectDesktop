#ifndef ITEMVIDEO_FORQOBUZ_H
#define ITEMVIDEO_FORQOBUZ_H

#include <QWidget>
#include "qobuz/qobuz_struct.h"
//include "AbstractItem_forQobuz.h"
#include "tidal/AbstractItem.h"     // TODO :: Added diskept 16/12/2020


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz 전용으로 사용하는 Video 아이템 UI
     */
    class ItemVideo_qobuz : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemVideo_qobuz(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);
        void setData(const qobuz::VideoItemData &p_data_video);

        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        qobuz::VideoItemData data_video;

        // ui
        QLabel *label_title;
        QLabel *label_creator;

    };

};


#endif // ITEMVIDEO_H
