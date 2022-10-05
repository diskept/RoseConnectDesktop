#ifndef TIDALALBUMTRACKDELEGATE_H
#define TIDALALBUMTRACKDELEGATE_H

#include "delegate/AbstractTidalTrackDelegate.h"
#include <QPainter>

namespace tidal {

    /**
     * @brief Tidal 앨범 내의 트랙 리스트 관련 delegate
     */
    class TidalAlbumTrackDelegate : public AbstractTidalTrackDelegate
    {
        Q_OBJECT

    public:
        explicit TidalAlbumTrackDelegate(int section, UseMode useMode, QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        QPixmap pixmap_play;
        QPixmap pixmap_mqa;
    };

};

#endif // TIDALALBUMTRACKDELEGATE_H
