#ifndef QOBUZTRACKDELEGATE_H
#define QOBUZTRACKDELEGATE_H

#include "delegate/AbstractTidalTrackDelegate.h"
#include <QPainter>

namespace qobuz {

    /**
     * @brief 일반 트랙 리스트 관련. 전체 트랙목록 (All View 에서). 아니면 PlayList의 트랙목록.
     * 트랙 아이템에는 image 부분이 포함되어 있음
     */
    class QobuzTrackDelegate : public tidal::AbstractTidalTrackDelegate
    {
        Q_OBJECT

    public:
        explicit QobuzTrackDelegate(int section, UseMode useMode, QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        QPixmap pixmap_hires;
    };

};

#endif // QOBUZTRACKDELEGATE_H
