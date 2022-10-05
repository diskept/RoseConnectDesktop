#ifndef QOBUZALBUMTRACKDELEGATE_H
#define QOBUZALBUMTRACKDELEGATE_H

#include "delegate/AbstractTidalTrackDelegate.h"
#include <QPainter>

namespace qobuz {

    /**
     * @brief Qobuz 앨범 내의 트랙 리스트 관련 delegate
     */
    class QobuzAlbumTrackDelegate : public tidal::AbstractTidalTrackDelegate
    {
        Q_OBJECT

    public:
        explicit QobuzAlbumTrackDelegate(int section, UseMode useMode, QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        QPixmap pixmap_play;
        QPixmap pixmap_hires;
    };

};

#endif // QOBUZALBUMTRACKDELEGATE_H
