#ifndef PLAYLISTITEM_FORQOBUZ_H
#define PLAYLISTITEM_FORQOBUZ_H

#include "qobuz/qobuz_struct.h"

#include "tidal/AbstractItem.h"     // TODO :: Added Jeon 16/12/2020


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz 전용으로 사용하는 Playlist 아이템 UI
     */
    class ItemPlaylist_qobuz : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemPlaylist_qobuz(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent = nullptr);

        void setData(const qobuz::PlaylistItemData &p_data_playList);
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // data
        qobuz::PlaylistItemData data_playlist;

        // ui
        QLabel *label_title;
        QLabel *label_creator;
        QLabel *label_hires;

    };

};

#endif // PLAYLISTITEM_H
