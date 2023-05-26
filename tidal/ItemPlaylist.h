#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include "tidal/AbstractItem.h"
#include "tidal/tidal_struct.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal 전용으로 사용하는 Playlist 아이템 UI
     */
    class ItemPlaylist : public AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemPlaylist(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const tidal::PlaylistItemData &p_data_playList);
        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        tidal::PlaylistItemData data_playlist;

        // ui
        QLabel *label_title;
        QLabel *label_creator;
        QLabel *label_mqa;
    };

};

#endif // PLAYLISTITEM_H
