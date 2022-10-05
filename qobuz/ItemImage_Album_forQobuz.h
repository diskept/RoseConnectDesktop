#ifndef ITEMIMAGE_ALBUM_FORQOBUZ_H
#define ITEMIMAGE_ALBUM_FORQOBUZ_H

//#include "AbstractItemImage_forQobuz.h"
#include <QWidget>
#include "tidal/AbstractItemImage.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {
    /**
     * @brief @sa ItemAlbum 에서 사용하는 Qobuz Album용 Image파트 클래스
     */
    class ItemImage_Album_qobuz : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Album_qobuz(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };

};
#endif // ITEMIMAGE_ALBUM_H
