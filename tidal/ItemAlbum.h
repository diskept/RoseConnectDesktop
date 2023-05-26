#ifndef ITEMALBUM_H
#define ITEMALBUM_H

#include "tidal/AbstractItem.h"
#include "tidal/tidal_struct.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemAlbum : public AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemAlbum(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);
        void setData(const tidal::AlbumItemData &p_data_album);                               ///< 실제 사용함. 타이달에서만. struct 사용.

        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        tidal::AlbumItemData data_album;

        // ui
        QLabel *label_title;
        QLabel *label_artist;
        QLabel *label_mqa;
    };

};

#endif // ITEMALBUM_H
