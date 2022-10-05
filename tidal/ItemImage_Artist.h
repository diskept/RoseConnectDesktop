#ifndef ITEMIMAGE_ARTIST_H
#define ITEMIMAGE_ARTIST_H

#include "AbstractItemImage.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {
    /**
     * @brief @sa ItemArtist 에서 사용하는 Tidal Artist용 Image파트 클래스
     */
    class ItemImage_Artist : public AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Artist(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_ARTIST_H
