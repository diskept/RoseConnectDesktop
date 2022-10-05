#ifndef QOBUZEDITTRACKDELEGATE_H
#define QOBUZEDITTRACKDELEGATE_H

#include "AbstractEditListDelegate.h"

namespace qobuz {

    /**
     * @brief Qobuz의 my playlist 내의 트랙 목록의 편집을 위한 delegate
     */
    class QobuzEditTrackDelegate : public AbstractEditListDelegate
    {
        Q_OBJECT
    public:
        explicit QobuzEditTrackDelegate(QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    };

};

#endif // QOBUZEDITTRACKDELEGATE_H
