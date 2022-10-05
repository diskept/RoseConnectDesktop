#ifndef ITEMARTIST_FORQOBUZ_H
#define ITEMARTIST_FORQOBUZ_H

#include "qobuz/qobuz_struct.h"

#include "tidal/AbstractItem.h"     // TODO :: Added Jeon 16/12/2020


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz 전용으로 사용하는 Artist 아이템 UI
     */
    class ItemArtist_qobuz : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemArtist_qobuz(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const qobuz::ArtistItemData &p_data_artist);                               ///< 실제 사용함. 타이달에서만. struct 사용.
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        qobuz::ArtistItemData data_artist;
        // ui
        QLabel *label_artist;

    };
};

#endif // ITEMARTIST_H
