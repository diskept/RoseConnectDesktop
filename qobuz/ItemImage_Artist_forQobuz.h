#ifndef ITEMIMAGE_ARTIST_FORQOBUZ_H
#define ITEMIMAGE_ARTIST_FORQOBUZ_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {
    /**
     * @brief @sa ItemArtist 에서 사용하는 Qobuz Artist용 Image파트 클래스
     */
    class ItemImage_Artist_qobuz : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Artist_qobuz(int imageSize, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_ARTIST_H
