#ifndef BUGSALBUMTRACKDELEGATE_H
#define BUGSALBUMTRACKDELEGATE_H

#include "delegate/AbstractTidalTrackDelegate.h"
#include <QPainter>

namespace bugs {

    /**
     * @brief Bugs의 앨범 내의 트랙 리스트 관련. 앨범 내 트랙은 pixmap 세팅이 없음
     */
    class BugsAlbumTrackDelegate : public tidal::AbstractTidalTrackDelegate
    {
        Q_OBJECT

    public:
        explicit BugsAlbumTrackDelegate(int section, UseMode useMode, QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        QPixmap pixmap_play;
    };

};

#endif // BUGSALBUMTRACKDELEGATE_H
