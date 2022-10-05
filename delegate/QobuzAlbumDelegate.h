#ifndef QOBUZALBUMDELEGATE_H
#define QOBUZALBUMDELEGATE_H

#include "delegate/AbstractTidalTrackDelegate.h"
#include <QPainter>

namespace qobuz {

    /**
     * @brief 일반 앨범 리스트 관련. 수신된 앨범 목록
     * 앨범 아이템에는 image 부분이 포함되어 있음
     */
    class QobuzAlbumDelegate : public tidal::AbstractTidalTrackDelegate
    {
        Q_OBJECT

    public:
        explicit QobuzAlbumDelegate(int section, UseMode useMode, QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        QPixmap pixmap_hires;
    };

};

#endif // QOBUZALBUMDELEGATE_H
