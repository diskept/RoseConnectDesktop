#ifndef ITEMIMAGE_ALBUM_H
#define ITEMIMAGE_ALBUM_H

#include "AbstractItemImage.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {
    /**
     * @brief @sa ItemAlbum 에서 사용하는 Tidal Album용 Image파트 클래스
     */
    class ItemImage_Album : public AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Album(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};
#endif // ITEMIMAGE_ALBUM_H
