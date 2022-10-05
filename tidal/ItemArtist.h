#ifndef ITEMARTIST_H
#define ITEMARTIST_H

#include "tidal/AbstractItem.h"
#include "tidal/tidal_struct.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal 전용으로 사용하는 Artist 아이템 UI
     */
    class ItemArtist : public AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemArtist(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const tidal::ArtistItemData &p_data_album);        ///< 실제 사용함. 타이달에서만. struct 사용.
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        tidal::ArtistItemData data_artist;

        // ui
        QLabel *label_artist;

    };
};

#endif // ITEMARTIST_H
