#ifndef TIDALEDITTRACKDELEGATE_H
#define TIDALEDITTRACKDELEGATE_H

#include "AbstractEditListDelegate.h"

namespace tidal {

    /**
     * @brief Tidal의 my playlist 내의 트랙 목록의 편집을 위한 delegate
     */
    class TidalEditTrackDelegate : public AbstractEditListDelegate
    {
        Q_OBJECT
    public:
        explicit TidalEditTrackDelegate(QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    };

};

#endif // TIDALEDITTRACKDELEGATE_H
