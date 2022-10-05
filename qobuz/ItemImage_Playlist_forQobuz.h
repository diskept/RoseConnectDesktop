#ifndef ITEMIMAGE_PLAYLIST_FORQOBUZ_H
#define ITEMIMAGE_PLAYLIST_FORQOBUZ_H

//#include "AbstractItemImage_forQobuz.h"//cheon2 code
#include <QWidget>
#include "tidal/AbstractItemImage.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {
    /**
     * @brief @sa ItemPlaylist 에서 사용하는 Qobuz playlist용 Image파트 클래스
     */
    class ItemImage_Playlist_qobuz : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Playlist_qobuz(int imageWidth, int imageHeight, QWidget *parent = nullptr);

    protected:
        QString getImagePath_default() override;

    };
};

#endif // ITEMIMAGE_PLAYLIST_H
