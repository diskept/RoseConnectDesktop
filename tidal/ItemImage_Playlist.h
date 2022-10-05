#ifndef ITEMIMAGE_PLAYLIST_H
#define ITEMIMAGE_PLAYLIST_H

#include "AbstractItemImage.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {
    /**
     * @brief @sa ItemPlaylist 에서 사용하는 Tidal playlist용 Image파트 클래스
     */
    class ItemImage_Playlist : public AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Playlist(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_PLAYLIST_H
